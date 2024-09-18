import csim
from gym import spaces
from gym.utils import seeding
from time import localtime, strftime
import numpy as np
import copy
import sys
import matplotlib.pyplot as plt
from norm import Normalizer
from collections import deque

#SCENARIO = "Basic"
#SCENARIO = "Passing"
#SCENARIO = "Dot"
SCENARIO = "Hallway"

class Env:
	def __init__(self):

		self.actions = np.array([], dtype=np.float32)
		self.AGENT_NUM = 42
		self.OBSTACLE_NUM = 0
		self.Parser = csim.Parser(SCENARIO, self.AGENT_NUM, self.OBSTACLE_NUM)
		body_dim = 18
		vision_dim = 36
		vel_dim = 36
		dim = body_dim + vision_dim + vel_dim

		ob_low = 0.0 * np.ones(dim) # 14 + 45 + 45 = 104
		ob_high =1.0 * np.ones(dim)

		self.normalizer_ac = Normalizer(
			real_val_max=1.0*np.ones(3),
			real_val_min=-1.0*np.ones(3),
			norm_val_max=3*np.ones(3),
			norm_val_min=-3*np.ones(3)
		)

		for i in range(2):
			ob_low[i] = -1.0

		ob_low[3] = -1.0
		ob_high[3] = 1.0
		ob_low[4] = -0.5
		ob_high[4] = 2.0
		ob_low[4] = -1.5
		ob_high[4] = 1.5

		# vel observation
		for i in range(body_dim+vision_dim, dim):
			ob_low[i] = -3.0
			ob_high[i] = 3.0

		self.observation_space = spaces.Box(
			low = ob_low,
			high = ob_high
		)

		self.action_space = spaces.Box(
			low = self.normalizer_ac.norm_val_min,
			high = self.normalizer_ac.norm_val_max
		)

		self.seed()
		self.reward_range = (-float('inf'), float('inf'))
		self.metadata = {'render.modes' : []}
		self.spec = None

		self.cur_step = 0
		self.cur_step2 = 0
		self.initPlot()
		
		self.highest_mean_reward_cum = -7877678
		self.highest_mean_reward_target_cum = 0
		self.highest_mean_reward_prefV_cum = 0
		self.highest_mean_reward_smooth_cum = 0
		self.highest_mean_reward_col_cum = -7877678
		self.highest_mean_reward_dir_cum = 0
		self.highest_mean_reward_comfortDist_cum = 0

		self.highest_mean_reward_col_cum_original = 0

		self.reward_cum_diff_btwn_two_iterations = 0
		self.previous_reward_cum = 0
	
	def initPlot(self):
		self.fig = plt.figure()
		self.ax1 = self.fig.add_subplot(111)
		self.ax1.plot([], label='reward', color='black')
		self.ax1.plot([], label='target', color='red')
		self.ax1.plot([], label='pref', color='green')
		self.ax1.plot([], label='smooth', color='blue')
		self.ax1.plot([], label='col', color='purple')
		self.ax1.plot([], label='dir', color='cyan')
		self.ax1.plot([], label='comfortDist', color='pink')
		self.ax1.legend()

		self.resetRewardCum()

		self.reward_queue = deque(maxlen=100)
		self.target_queue = deque(maxlen=100)
		self.prefV_queue = deque(maxlen=100)
		self.smooth_queue = deque(maxlen=100)
		self.col_queue = deque(maxlen=100)
		self.dir_queue = deque(maxlen=100)
		self.comfortDist_queue = deque(maxlen=100)

		self.reward_plot = []
		self.target_plot = []
		self.prefV_plot = []
		self.smooth_plot = []
		self.col_plot = []
		self.dir_plot = []
		self.comfortDist_plot = []

	def resetRewardCum(self):
		self.reward_cum = 0
		self.reward_target_cum = 0
		self.reward_prefV_cum = 0
		self.reward_smooth_cum = 0
		self.reward_col_cum = 0
		self.reward_dir_cum = 0
		self.reward_comfortDist_cum = 0
		

	def setRewardPlot(self, reward, rewards):
		#print('\nfrom env.py, self.reward_target_cum= ', self.reward_target_cum, "self.reward_smooth_cum= ", self.reward_smooth_cum, "self.reward_col_cum= ", self.reward_col_cum, "\n")
		self.reward_target_cum += rewards[0]
		self.reward_prefV_cum += rewards[1]
		self.reward_smooth_cum += rewards[2]
		self.reward_col_cum += rewards[3]
		self.reward_dir_cum += rewards[4]
		self.reward_comfortDist_cum += rewards[5]

		self.reward_cum += rewards[0]+rewards[1]+rewards[2]+rewards[3]+rewards[4] + rewards[5]
		#print('\nfrom env.py, reward_col_cum= ', self.reward_col_cum, '\n')

	def appendPlot(self):
		self.reward_queue.append(self.reward_cum)
		#print('length of reward_queue: ', len(self.reward_queue), "\n" )
		self.target_queue.append(self.reward_target_cum)
		self.prefV_queue.append(self.reward_prefV_cum)
		self.smooth_queue.append(self.reward_smooth_cum)
		self.col_queue.append(self.reward_col_cum)

		self.dir_queue.append(self.reward_dir_cum)
		self.comfortDist_queue.append(self.reward_comfortDist_cum)

		#print('np.mean(self.col_queue)= ', np.mean(self.col_queue), '\n')
		if((np.mean(self.col_queue)) > (self.highest_mean_reward_col_cum)):
			self.highest_mean_reward_col_cum_original = np.mean(self.col_queue)
			#print("self.highest_mean_reward_col_cum_original= ", self.highest_mean_reward_col_cum_original, "\n")

		if((np.mean(self.reward_queue)) > (self.highest_mean_reward_cum)):
			self.highest_mean_reward_cum = np.mean(self.reward_queue)
			self.highest_mean_reward_target_cum = np.mean(self.target_queue)
			self.highest_mean_reward_prefV_cum = np.mean(self.prefV_queue)
			self.highest_mean_reward_smooth_cum = np.mean(self.smooth_queue)
			self.highest_mean_reward_col_cum = np.mean(self.col_queue)
			self.highest_mean_reward_dir_cum = np.mean(self.dir_queue)
			self.highest_mean_reward_comfortDist_cum = np.mean(self.comfortDist_queue)
		
		return self.highest_mean_reward_cum, self.highest_mean_reward_target_cum, self.highest_mean_reward_prefV_cum, self.highest_mean_reward_smooth_cum, self.highest_mean_reward_col_cum, self.highest_mean_reward_dir_cum, self.highest_mean_reward_comfortDist_cum   #extra added later

	def saveValuesForPlotting(self):
		self.reward_plot.append(np.mean(self.reward_queue))
		self.target_plot.append(np.mean(self.target_queue))
		self.prefV_plot.append(np.mean(self.prefV_queue))
		self.smooth_plot.append(np.mean(self.smooth_queue))
		self.col_plot.append(np.mean(self.col_queue))
		self.dir_plot.append(np.mean(self.dir_queue))
		self.comfortDist_plot.append(np.mean(self.comfortDist_queue))

	def drawPlot(self):

		#plt.ion()
		self.ax1.plot(self.reward_plot, label='reward', color='black')
		plt.draw()


	def plotSave(self):

		plt.savefig("../../data/ckpt/network/graphPhotos/modelHallway23stFeb23_2/modelHallway23stFeb23_2"+str(strftime("%Y%m%d_%I%M", localtime())))
		self.ax1.cla() #https://github.com/matplotlib/matplotlib/issues/20300#issuecomment-847921543
# =================================================
	def save_rewards_to_file(self):

		self.reward_cum_diff_btwn_two_iterations = self.reward_cum - self.previous_reward_cum
		self.previous_reward_cum = self.reward_cum

		#self.appendPlot()
		f = open("../../data/ckpt/network/graphPhotos/modelHallway23stFeb23_2/modelHallway23stFeb23_2.txt", "a")
		f.write("cur_step: %s\n" % (self.cur_step) )

		f.write("#highest_mean_reward_cum, highest_mean_reward_target_cum, highest_mean_reward_prefV_cum, highest_mean_reward_smooth_cum, highest_mean_reward_col_cum, highest_mean_reward_dir_cum, highest_mean_reward_comfortDist_cum\n")	
		f.write("#reward_cum, target_cum, prefV_cum, smooth_cum, col_cum, dir_cum, comfortDist_cum\n")
		f.write("reward_cum: %s, \nreward_target: %s, \nreward_prefV: %s, \nreward_smooth: %s, \nreward_col: %s, \nreward_dir: %s, \nreward_comfortDist: %s\n\n" % (self.highest_mean_reward_cum, self.highest_mean_reward_target_cum, self.highest_mean_reward_prefV_cum, self.highest_mean_reward_smooth_cum, self.highest_mean_reward_col_cum, self.highest_mean_reward_dir_cum, self.highest_mean_reward_comfortDist_cum) )
		f.write("highest_mean_reward_col_cum_original: %s\n" % (self.highest_mean_reward_col_cum_original) )

		f.write("mean_reward_cum: %s\n" % (np.mean(self.reward_queue)) )
		f.write("reward_cum: %s, "  ", self.previous_reward_cum: %s\n" % (self.reward_cum, self.previous_reward_cum) )
		f.write("reward_cum_diff_btwn_two_iterations: %s\n\n\n\n\n" % (self.reward_cum_diff_btwn_two_iterations) )

		f.close()

# =================================================

	def step(self, a):
		self.steps([a])
		#self.actions = np.array([], dtype=np.float32)
		memory = self.Parser.Step(self.actions, False)	
		#print("from step() of env.py, self.actions: ", self.actions, "\n")
		obs = memory['obs']
		obs = self.convert_to_numpy(obs)

		body_state = obs['agent'][0]['body_state']
		#print("body_state from step() of env.py, v[0]: ", body_state[3], ",  v[1]", body_state[4], "\n")
		sensor_state = obs['agent'][0]['sensor_state']
		velocity_state = obs['agent'][0]['velocity_state']
		state = np.concatenate((body_state, sensor_state, velocity_state), axis=None)
		reward = memory['reward']
		isTerm = memory['isTerm']

		if np.isnan(state).any():
			print("state nan")
			sys.exit(1)

		rewards = memory['reward_sep']
		self.cur_step += 1
		
		self.setRewardPlot(reward, rewards)

		if isTerm:
			self.appendPlot()
			self.saveValuesForPlotting()
			self.resetRewardCum()
			if self.cur_step >= 5120000:
				self.drawPlot()
		
		if(self.cur_step >= 5120000):
			self.save_rewards_to_file()
		
		return state, reward, isTerm, {}

	def steps(self, a_):
		a_norm = []
		for ac in a_:
			a_norm.append(self.normalizer_ac.norm_to_real(ac))

		self.actions =np.array(a_norm, dtype=np.float32)

	def step_render(self, a_):
		a_norm = []
		for ac in a_:
			a_norm.append(self.normalizer_ac.norm_to_real(ac))
			# if ac[0] > 3.0 or ac[1] > 3.0 or ac[2] > 3.0:
			# print("ac : ", ac)
			# print("ac_ : ", self.normalizer_ac.norm_to_real(ac))

		memory = self.Parser.Step(np.array(a_norm, dtype=np.float32), True)
		obs = memory['obs']
		obs = self.convert_to_numpy(obs)

		state = obs
		reward = memory['reward']
		isTerm = memory['isTerm']

		return state, reward, isTerm, {}

	def states(self):

		obs = self.Parser.Observe()
		obs = self.convert_to_numpy(obs)
		states = []
		for i in range(len(obs['agent'])):
			body_state = obs['agent'][i]['body_state']
			sensor_state = obs['agent'][i]['sensor_state']
			velocity_state = obs['agent'][i]['velocity_state']
			state = np.concatenate((body_state, sensor_state, velocity_state), axis=None)
			states.append(state)

		return states

	def seed(self, seed=None):
		self.np_random, seed = seeding.np_random(seed)
		return [seed]

	def reset(self):
		self.Parser.Reset(-1, self.AGENT_NUM, self.OBSTACLE_NUM)
		obs = self.Parser.Observe()
		obs = self.convert_to_numpy(obs)

		body_state = obs['agent'][0]['body_state']
		sensor_state = obs['agent'][0]['sensor_state']
		velocity_state = obs['agent'][0]['velocity_state']
		state = np.concatenate((body_state, sensor_state, velocity_state), axis=None)

		return state

	def observe(self):
		obs = self.Parser.Observe()
		obs = self.convert_to_numpy(obs)

		return obs

	def convert_to_numpy(self, obs):
		for i in range(self.AGENT_NUM): # r, p, d, front, color
			r_data = obs['agent'][i]['render_data']
			obs['agent'][i]['r'] = np.array([r_data[0], r_data[1]])
			obs['agent'][i]['p'] = np.array([r_data[2], r_data[3]])
			obs['agent'][i]['d'] = np.array([r_data[4], r_data[5]])
			obs['agent'][i]['front'] = r_data[6]
			obs['agent'][i]['color'] = np.array([r_data[7], r_data[8], r_data[9]])
			obs['agent'][i]['body_state'] = np.array(obs['agent'][i]['body_state'])
			obs['agent'][i]['sensor_state'] = np.array(obs['agent'][i]['sensor_state'])
			obs['agent'][i]['velocity_state'] = np.array(obs['agent'][i]['velocity_state'])
			obs['agent'][i]['offset'] = np.array(obs['agent'][i]['offset_data'])

		obstacle_num = len(obs['obstacle'])
		for i in range(obstacle_num):
			obs['obstacle'][i]['p'] = np.array(obs['obstacle'][i]['p'])
			obs['obstacle'][i]['r'] = np.array(obs['obstacle'][i]['r'])
			obs['obstacle'][i]['front'] = np.array(obs['obstacle'][i]['front'])

		wall_num = len(obs['wall'])
		for i in range(wall_num):
			obs['wall'][i]['p'] = np.array(obs['wall'][i]['p'])
			obs['wall'][i]['w'] = obs['wall'][i]['w']
			obs['wall'][i]['h'] = obs['wall'][i]['h']

		return obs
