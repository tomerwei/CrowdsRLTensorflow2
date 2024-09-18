import sys
sys.path.append('../base')
sys.path.append('scenarios')
sys.path.append('baselines')

import os
import tensorflow as tf

config = tf.compat.v1.ConfigProto(log_device_placement=True)
session = tf.compat.v1.Session(config=config)

try:
	from mpi4py import MPI
except ImportError:
	MPI = None

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

from baselines.common import tf_util as U
# from baselines.deepq import utils as U2

from baselines import logger
from baselines.bench import Monitor
from baselines.common import set_global_seeds

import os
import csim
import numpy as np
import math
from time import localtime, strftime
from constants import Constants as cst

from env import Env
from dot import Dot
from basic import Basic
from passing import Passing
from hallway import Hallway

#SCENARIO = 'Basic'
#SCENARIO = 'Passing'
#SCENARIO = 'Dot'
SCENARIO = 'Hallway'

class Renderer:
	def __init__(self, WIDTH=1280, HEIGHT=720):
		
		self.WIDTH = WIDTH
		self.HEIGHT = HEIGHT
		self.initGL()
		self.initFlag()

		self.SCENARIO = SCENARIO

		self.isTerm = False
		self.fps = 120
		self.pi = self.train(num_timesteps=1)
		#print("printing from _init__() of render.py ")
		#U.load_state("../data/ckpt/network/0102/8.0")		#i have commented that line
		#U.load_state("/home/sagarbilas/CrowdSimulation_jaedong_3/srcs/src/modelseptember2")		# loading the modelseptember2 from the specified path. trained for 400 iterations
		
		#U.load_state("/home/sagarbilas/CrowdSimulation_jaedong_3/srcs/src/models/modelHallway28thMar2")  # working good
		#U.load_state("/home/sagarbilas/CrowdSimulation_jaedong_3/srcs/src/models/modelHallway27thMar6")  # Baseline model
		#U.load_state("/home/sagarbilas/CrowdSimulation_jaedong_3/srcs/src/models/modelHallway27thApr2")
		#U.load_state("/home/sagarbilas/CrowdSimulation_jaedong_3/srcs/src/models/modelHallway13thApr2")
		#U.load_state("/home/sagarbilas/CrowdSimulation_jaedong_3/srcs/src/models/modelHallway13thMar4")  # working good
		#U.load_state("/home/sagarbilas/CrowdSimulation_jaedong_3/srcs/src/models/modelHallway27thApr3")  # working good
		#U.load_state("/home/sagarbilas/CrowdSimulation_jaedong_3/srcs/src/models/modelHallway8thApr3")    #commented on 04-25-2022. working good

		U.load_state("models/modelHallway23stFeb23_2")
		#U.load_state("/home/sagarbilas/1_copied/CrowdSimulation_jaedong_3_a2/srcs/src/models/modelHallway21thAug22_4")

		#U.load_state("/home/sagarbilas/CrowdSimulation_jaedong_3/srcs/src/models/modelHallway25thApr1")   # Baseline model
		#U.load_state("/home/sagarbilas/CrowdSimulation_jaedong_3/srcs/src/models/modelHallway25thApr2")   # Baseline model
		
		
		self.env = self.make_env()
		ob = self.env.reset()
		self.setEnvironment(SCENARIO)

		self.timer_func()
		glutMainLoop()

	def initGL(self):
		argv = sys.argv
		glutInit(argv)
		glutInitWindowPosition(0,0)
		glutInitWindowSize(self.WIDTH, self.HEIGHT)
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH)

		self.windowID = glutCreateWindow("Crowd Simulation")
		glutDisplayFunc(self.display)
		glutReshapeFunc(self.reshape)
		glutKeyboardFunc(self.keyCB)

	def initFlag(self):
		self.flag={}
		self.flag['quit'] = False
		self.flag['play'] = False
		self.flag['done'] = False
		self.flag['vision'] = False

	def setEnvironment(self, SCENARIO):
		self.Parser = csim.Parser(SCENARIO,0,0)

		obs = self.env.env.observe()

		if SCENARIO == 'Basic':
			self.Scenario = Basic(obs)
		elif SCENARIO == 'Passing':
			self.Scenario = Passing(obs)
		elif SCENARIO == 'Dot':
			self.Scenario = Dot(obs)
		elif SCENARIO == 'Hallway':
			self.Scenario = Hallway(obs)

	def make_env(self, seed=None):
		reward_scale = 1.0

		rank = MPI.COMM_WORLD.Get_rank()
		myseed = seed + 1000 * rank if seed is not None else None
		set_global_seeds(myseed)
		env = Env()

		logger_path = None if logger.get_dir() is None else os.path.join(logger.get_dir(), str(rank))
		print("logger path is: ", logger_path, "\n\n\n\n")
		env = Monitor(env, logger_path, allow_early_resets=True)
		env.seed(seed)
		if reward_scale != 1.0:
			from baselines.common.retro_wrappers import RewardScaler
			env = RewardScaler(env, reward_scale)
		return env

	def train(self, num_timesteps):
		from baselines.ppo1 import mlp_policy, pposgd_simple
		sess = U.make_session(num_cpu=1)
		sess.__enter__()
		
		def policy_fn(name, ob_space, ac_space):
			return mlp_policy.MlpPolicy(name=name, ob_space=ob_space, ac_space=ac_space,
				#hid_size=128, num_hid_layers=3)
				hid_size=64, num_hid_layers=3)					# hid_size was 128. I changed it to 64
		env = self.make_env()
		
		pi = pposgd_simple.learn(env, policy_fn,
				max_timesteps=num_timesteps,
				timesteps_per_actorbatch=512,
				clip_param=0.2, entcoeff=0.0,
				optim_epochs=10,
				optim_stepsize=3e-4,
				optim_batchsize=64,
				gamma=0.95,
				lam=0.95,
				schedule='linear',
			)
		return pi
		
	def timer_func(self, fps=120):
		if self.flag['play']:
			if not self.flag['done']:
				obs = self.env.env.states()
				action_ = []
				for i in range(len(obs)):
					#print(self.pi)
					action_.append(self.pi.act(False, obs[i])[0])
				action_ = np.array(action_)
				ob, _, done, _ = self.env.env.step_render(action_)
				self.Scenario.setObjectData(ob)
				if done:
					self.flag['done'] = True

		glutPostRedisplay()
		glutTimerFunc(int(2000/fps), self.timer_func, fps)	# was till 08-29-2022

	def keyCB(self, key, x, y):
		print(key)
		if key:
			if key == b'q':
				self.flag['quit'] = True
				glutDestroyWindow (self.windowID)
				exit (0)        #newly added
			elif key == b' ':
				if self.flag['play']:
					print("STOP")
				else:
					print("PLAY")
				self.flag['play'] = not self.flag['play']
			elif key == b'r':
				ob = self.env.reset()
				self.flag['done'] = False
				self.flag['play'] = False
				print("reset")
			elif key == b'e':
				self.flag['vision'] = not self.flag['vision']

	def reshape(self, w, h):
		glViewport(0, 0, w, h)
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity()
		glOrtho(-cst.WIDTH, cst.WIDTH, -cst.HEIGHT, cst.HEIGHT, -cst.DEPTH, cst.DEPTH)
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity()

	def render_base(self):
		for i in range(12):
			for j in range(12):
				glPushMatrix()
				glTranslatef(-36 + i * 6 + 3, -36 + j * 6 + 3 , 0)
				if (i+j) % 2 ==0:
					#glColor3f(0.85, 0.85, 0.85)    # to make backgound black
					glColor3f(0.0, 0.0, 0.0)
				else:
					#glColor3f(0.95, 0.95, 0.95)    # to make backgound black
					glColor3f(0.0, 0.0, 0.0)
				glBegin(GL_QUADS)
				glVertex3f( -8.0,  -8.0, 0)
				glVertex3f( -8.0,   8.0, 0)
				glVertex3f(  8.0,   8.0, 0)
				glVertex3f(  8.0,  -8.0, 0)
				glEnd()
				glPopMatrix()

	def display(self):
		glClearColor(0.8, 0.8, 0.8, 0.0)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity()

		gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0)

		glPushMatrix()
		self.render_base()
		glPopMatrix()

		glPushMatrix()
		self.Scenario.render(vision = self.flag['vision'], trajectory = False)
		glPopMatrix()

		glutSwapBuffers()
	
if __name__=="__main__":
	ren_ = Renderer(cst.WINDOW_WIDTH, cst.WINDOW_HEIGHT)
	#self.main()
