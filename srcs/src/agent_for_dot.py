import sys
sys.path.append('../base')

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

from crowdobject import CrowdObject
from constants import Constants as cst

import numpy as np
import mMath
import math
import copy
import random

class Agent(CrowdObject):
	def __init__(self, state, color='RED'):
		self.reset(state)

	def getState(self):
		state = {}
		state['r'] = self.r
		state['p'] = self.p
		state['q'] = self.q
		state['d'] = self.d
		state['color'] = self.color
		state['front'] = self.front
		state['vision'] = self.vision
		#state['vision_vel'] = self.vision_vel
		state['offset'] = self.offset
		
		#  To add line using last 50 positions of agents.
		#state['lastFiftyPosXv'] = self.lastFiftyPosXv
		#state['lastFiftyPosYv'] = self.lastFiftyPosYv

		return copy.copy(state)

	def setR(self, r = None):
		if r is None:
			r = [0.5, 0.5]
		self.r = np.array(r, dtype=float)

	def setP(self, p = None):
		if p is None:
			p = [-200, 0]
		self.p = np.array(p, dtype=float)

	def setD(self, d = None):
		if d is None:
			d = [0, 0]
		self.d = np.array(d, dtype=float)

	def setColor(self, color = None):
		self.color = np.array(color, dtype=float)

	def setFront(self, f = None):
		if f is None:
			f = 0.0

		self.front = f

	def setVision(self, v = None):
		if v is None:
			v = np.ndarray(shape=(45))
			v.fill(0)

		self.vision = v
		
	def setAnimationStatus(self, animStatus = None):
		if animStatus is None:
			animStatus = 0
		self.animStatus = animStatus
		
	def getAnimationStatus(self):
		return self.animStatus
	
	'''
	def setVisionVel(self, v = None):
		if v is None:
			v = np.ndarray(shape=(3, 41))
			v.fill(0)

		self.vision_vel = v
	'''

	def setOffset(self, o = None):
		if o is None:
			o = np.ndarray(shape=(41))
			o.fill(0)

		self.offset = o
		
	def setLastFiftyPosXv(self, lastFiftyPosXv = None):
		if lastFiftyPosXv is None:
			lastFiftyPosXv = np.ndarray(shape=(50))
			lastFiftyPosXv.fill(0)
		self.lastFiftyPosXv = np.array(lastFiftyPosXv, dtype=float)
		#self.lastFiftyPosXv = lastFiftyPosXv
		
	def setLastFiftyPosYv(self, lastFiftyPosYv = None):
		if lastFiftyPosYv is None:
			lastFiftyPosYv = np.ndarray(shape=(50))
			lastFiftyPosYv.fill(0)
		self.lastFiftyPosYv = np.array(lastFiftyPosYv, dtype=float)	
		#self.lastFiftyPosYv = lastFiftyPosYv

	def reset(self, state):
		self.r = state['r']
		self.p = state['p']
		self.d = state['d']
		self.color = state['color']
		self.front = state['front']
		self.offset = state['offset']
		self.setVision(state['vision'])
		#self.setVisionVel(state['vision_vel'])

		self.trajectory = []
		self.trajectory_q = []
		self.animStatus = 0
		self.pp = self.p
		self.pfront = self.front
		
		self.lastFiftyPosXv = []
		#self.lastFiftyPosYv = state['lastFiftyPosYv']

	def render(self, vision=False, trajectory_=False, idx=0, animate=0):
		#cout<<"p[0] is: " << self.p[0] <<"	" << "p[1] is: " << self.p[1]<<"\n"; 
		#print("idx from render() of agent.py: ", idx)
		
		#render Agent
		glPushMatrix()
		self.render_agent(idx, vision, animate)
		glPopMatrix()

		if trajectory_:
			glLineWidth(4.0)
			glColor3f(self.color[0],self.color[1],self.color[2])
			glBegin(GL_LINES)
			l = len(self.trajectory)

			for i in range(l):
				glVertex3f(self.trajectory[i][0], self.trajectory[i][1], 0.5)
			glEnd()

		self.render_destination()

	def render_agent(self, idx, vision, animate):
		#print("idx from render_agent() of agent.py: ", idx)
		# render agent
		glPushMatrix()
		if animate == 0:
			self.pp = self.p
			self.pfront = self.front
		glTranslatef(self.pp[0], self.pp[1], 1)
		glRotatef(mMath.RadianToDegree(self.pfront)-90, 0, 0, 1)
			

		if vision and idx == 0:
			self.render_vision()

		# agentlastFiftyPosXv[j
		glScalef(self.r[0], self.r[1], 1.0)
		glColor3f(self.color[0],self.color[1],self.color[2])
		quad = gluNewQuadric()
		gluSphere(quad, 1.0, 50, 50)

		# direction triangle
		glColor3f(0.3, 0.3, 1.0)
		glBegin(GL_TRIANGLES)
		glVertex3f( 0.9, 0.0, 2.0)
		glVertex3f( 0.0, 0.9, 2.0)
		glVertex3f(-0.9, 0.0, 2.0)
		glEnd()
		glPopMatrix()
		
		# To add line using last 50 positions of agents
		glPushMatrix()
		glColor3f(1.0, 0.3, 0.5)
		glLineWidth(2);
		glBegin(GL_LINES)
		for j in range(len(self.lastFiftyPosXv) - 1):
		#for j in range(len(self.lastFiftyPosXv) - 2 ,0,-1):
			#print("lastFiftyPosXv[]: ",j, self.lastFiftyPosXv[j], self.lastFiftyPosYv[j])
			
			#print("index: ", j, ", lastFiftyPosXv[j]: ", self.lastFiftyPosXv[j], ", lastFiftyPosYv[j]: ", self.lastFiftyPosYv[j])        	    #temporarily commented
			#print("index: ", j, ", lastFiftyPosXv[j+1]: ", self.lastFiftyPosXv[j+1], ", lastFiftyPosYv[j+1]: ", self.lastFiftyPosYv[j+1])        #temporarily commented
			if self.lastFiftyPosXv[j] == -1000:
				continue
			glVertex2f(self.lastFiftyPosXv[j] ,self.lastFiftyPosYv[j])
			glVertex2f(self.lastFiftyPosXv[j+1] ,self.lastFiftyPosYv[j+1])
		glEnd()
		glPopMatrix()
		

	def render_destination(self):
		#print("from render_destination() of agent.py: ")
		glColor3f(1.0, 1.0, 0.0)
		glPushMatrix()
		glTranslatef(self.d[0], self.d[1], 3)
		quad = gluNewQuadric()
		gluSphere(quad, 0.3, 50, 50)
		glPopMatrix()
	'''
	def render_vision(self):
		vision_depth = 7.0/3.0
		glLineWidth(2)
		for i in range(36):
			angle = mMath.DegreeToRadian((i+0.5)*10.0 - 180.0 + 90.0)
			if self.vision_vel[i] < -3.0:
				self.vision_vel[i] = -3.0
			if self.vision_vel[i] > 3.0:
				self.vision_vel[i] = 3.0

			if self.vision_vel[i] < 0.0:
				glColor3f(0.7 + 0.1*self.vision_vel[i], 0.3 - 0.1*self.vision_vel[i], 0.0)
			elif self.vision_vel[i] > 0.0:
				glColor3f(0.3 + 0.1*self.vision_vel[i], 0.7 - 0.1*self.vision_vel[i], 0.0)
			else:
				glColor3f(0.5, 0.5, 0.0)

			glBegin(GL_LINES)
			glVertex3f(self.offset[i]*math.cos(angle), self.offset[i]*math.sin(angle), 0)
			glVertex3f(vision_depth*self.vision[i]*math.cos(angle), vision_depth*self.vision[i]*math.sin(angle), 0)
			glEnd()
	'''

class Obstacle(CrowdObject):
	def __init__(self, state):
		self.reset(state)

	def reset(self, state):
		self.p = state['p']
		self.r = state['r']
		self.front = state['front']

	def setP(self, p = None):
		if p is None:
			p = [0.0, 0.0]

		self.p = np.array(p, dtype=float)

	def setR(self, r = None):
		if r is None:
			r = [0.5, 0.5]

		self.r = np.array(r, dtype=float)

	def setFront(self, f = None):
		if f is None:
			f = 0.0

		self.front = f

	def getState(self):
		state = {}
		state['p'] = self.p
		state['r'] = self.r
		state['front'] = self.front
		return copy.deepcopy(state)

	def action(self, action):
		return

	def render(self):
		#render Obstacle
		glPushMatrix()
		glColor3f(0.2, 0.2, 0.25)
		glTranslatef(self.p[0], self.p[1], 0)
		glRotatef(mMath.RadianToDegree(self.front)-90, 0, 0, 1)
		glScalef(self.r[0], self.r[1], 1.0)

		quad = gluNewQuadric()
		gluSphere(quad, 1.0, 50, 50)
		glPopMatrix()

#Rectangle Form
class Wall():
	def __init__(self, state):
		self.reset(state)

	def reset(self, state):
		self.p = state['p']
		self.w = state['w']
		self.h = state['h']

	def setP(self, p = None):
		if p is None:
			p = [0.0, 0.0]

		self.p = np.array(p, dtype=float)

	def setW(self, w = None):
		if w is None:
			r = 1.0

		self.w = w

	def setH(self, h = None):
		if h is None:
			h = 1.0

		self.h = h

	def getState(self):
		state = {}
		state['p'] = self.p
		state['w'] = self.w
		state['h'] = self.h
		return copy.deepcopy(state)

	def render(self):
		glPushMatrix()
		glColor3f(0.2, 0.2, 0.25)
		#glColor3f(1.0, 1.0, 1.0)       # this will change the wall color of the rectangular areas of the output window to white
		glTranslatef(self.p[0], self.p[1], 0.0)
		glScalef(self.w, self.h, 1.0)
		#glutSolidCube(1.0)      #this  was original line
		#glutSolidCube(0.2)
		glPopMatrix()


