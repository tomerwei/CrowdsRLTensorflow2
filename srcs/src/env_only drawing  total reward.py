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
		self.AGENT_NUM = 20
		self.OBSTACLE_NUM = 0
		#print("parser is going to be called from env.py")
		self.Parser = csim.Parser(SCENARIO, self.AGENT_NUM, self.OBSTACLE_NUM)
		#print("parser has already been called from env.py")

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
		self.initPlot()
# ================plot=============================

	'''
	<! just rough lines>
	fig, (ax1, ax2) = plt.subplots(2)
	fig.suptitle('Axes values are scaled individually by default')
	ax1.plot(x, y)
	ax2.plot(x + 1, -y)
	'''
	def initPlot(self):
		self.fig = plt.figure()
		self.ax1 = self.fig.add_subplot(111)
		#self.ax1, self.ax2 = self.fig.add_subplot(111)
		self.ax1.plot([], label='reward', color='black')

		self.ax1.legend()

		self.resetRewardCum()

		self.reward_queue = deque(maxlen=100)
		self.target_queue = deque(maxlen=100)
		self.prefV_queue = deque(maxlen=100)
		self.smooth_queue = deque(maxlen=1000)
		self.col_queue = deque(maxlen=1000)
		self.bubble_queue = deque(maxlen=1000)
		self.dir_queue = deque(maxlen=1000)

		self.reward_plot = []
		self.target_plot = []
		self.prefV_plot = []
		self.smooth_plot = []
		self.col_plot = []
		self.bubble_plot = []
		self.dir_plot = []

	def resetRewardCum(self):
		self.reward_cum = 0
		self.reward_target_cum = 0
		self.reward_prefV_cum = 0
		self.reward_smooth_cum = 0
		self.reward_col_cum = 0
		self.reward_bubble_cum = 0
		self.reward_dir_cum = 0

	def setRewardPlot(self, reward, rewards):
		self.reward_target_cum += rewards[0]
		self.reward_prefV_cum += rewards[1]
		self.reward_smooth_cum += rewards[2]
		self.reward_col_cum += rewards[3]
		self.reward_bubble_cum += rewards[4]
		self.reward_dir_cum += rewards[5]

		self.reward_cum += rewards[0]+rewards[1]+rewards[2]+rewards[3]+rewards[4] + rewards[5]

	def appendPlot(self):
		self.reward_queue.append(self.reward_cum)
		self.target_queue.append(self.reward_target_cum)
		self.prefV_queue.append(self.reward_prefV_cum)
		self.smooth_queue.append(self.reward_smooth_cum)
		self.col_queue.append(self.reward_col_cum)
		self.bubble_queue.append(self.reward_bubble_cum)
		self.dir_queue.append(self.reward_dir_cum)

	def drawPlot(self):
		self.reward_plot.append(np.mean(self.reward_queue))

		#plt.ion()
		self.ax1.plot(self.reward_plot, label='reward', color='black')
		#self.ax1.plot(self.elapsedTime, label='elapsedTime', color='blue')
		plt.draw()
		#print("reward_plot", self.reward_plot)
		#plt.pause(0.1)
		#plt.show()

	def plotSave(self):
		#plt.savefig("../data/ckpt/network/graph"+str(strftime("%Y%m%d_%I%M", localtime())))
		#plt.savefig("../data/ckpt/network/graphPhotos/modelHallway19thDec1/modelHallway19thDec1"+str(strftime("%Y%m%d_%I%M", localtime())))
		plt.savefig("../data/ckpt/network/graphPhotos/modelHallway2ndMay1/modelHallway2ndMay1"+str(strftime("%Y%m%d_%I%M", localtime())))

# =================================================

	def step(self, a):
		#print("print from step() of env.py", a)
		self.steps([a])
		#self.actions = np.array([], dtype=np.float32)
		memory = self.Parser.Step(self.actions, False)	

		obs = memory['obs']
		obs = self.convert_to_numpy(obs)

		body_state = obs['agent'][0]['body_state']
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
			self.drawPlot()
			self.resetRewardCum()
		
		if(self.cur_step%1000 == 0):   #tried 
			self.plotSave()
		
		return state, reward, isTerm, {}

	def steps(self, a_):
		#print("print from steps() of env.py")
		a_norm = []
		for ac in a_:
			a_norm.append(self.normalizer_ac.norm_to_real(ac))

		self.actions =np.array(a_norm, dtype=np.float32)

	def step_render(self, a_):
		#print("from step_render() of env.py")
		a_norm = []
		for ac in a_:
			a_norm.append(self.normalizer_ac.norm_to_real(ac))
			# if ac[0] > 3.0 or ac[1] > 3.0 or ac[2] > 3.0:
			# print("ac : ", ac)
			# print("ac_ : ", self.normalizer_ac.norm_to_real(ac))

		memory = self.Parser.Step(np.array(a_norm, dtype=np.float32), True)

		obs = memory['obs']
		obs = self.convert_to_numpy(obs)

		# print("action : ", a_norm)
		# print("state : ", obs['agent'][0]['body_state'])

		state = obs
		reward = memory['reward']
		isTerm = memory['isTerm']

		return state, reward, isTerm, {}

	def states(self):
		#print("from the beginning of states() of env.py")
		obs = self.Parser.Observe()
		#print("from the end of states() of env.py")
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
		#print("from beginning of reset() of env.py")
		self.Parser.Reset(-1, self.AGENT_NUM, self.OBSTACLE_NUM)
		obs = self.Parser.Observe()
		#print("from end of reset() of env.py")
		obs = self.convert_to_numpy(obs)

		body_state = obs['agent'][0]['body_state']
		sensor_state = obs['agent'][0]['sensor_state']
		velocity_state = obs['agent'][0]['velocity_state']
		state = np.concatenate((body_state, sensor_state, velocity_state), axis=None)

		return state

	def observe(self):
		#print("from the beginning of observe() of env.py")
		obs = self.Parser.Observe()
		#print("from the end of observe() of env.py")
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
		#print("number of obstacles: ", obstacle_num)
		for i in range(obstacle_num):
			obs['obstacle'][i]['p'] = np.array(obs['obstacle'][i]['p'])
			#print("Position of Obstracle from env.py: ", obs['obstacle'][i]['p'])           # commented this while running new hallway model
			obs['obstacle'][i]['r'] = np.array(obs['obstacle'][i]['r'])
			obs['obstacle'][i]['front'] = np.array(obs['obstacle'][i]['front'])
			#print("Radius of Obstracle from env.py: ", obs['obstacle'][i]['r'])

		wall_num = len(obs['wall'])
		for i in range(wall_num):
			obs['wall'][i]['p'] = np.array(obs['wall'][i]['p'])
			obs['wall'][i]['w'] = obs['wall'][i]['w']
			obs['wall'][i]['h'] = obs['wall'][i]['h']

		return obs

