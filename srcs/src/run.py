import os
import tensorflow as tf

import csim
import sys

sys.path.append('../base')
sys.path.append('scenarios')
sys.path.append('baselines')

try:
    from mpi4py import MPI
except ImportError:
    MPI = None

import sys

sys.path.append('baselines')

from baselines.common import tf_util as U
from baselines import logger
from baselines.bench import Monitor
from baselines.common import set_global_seeds

from time import localtime, strftime

from env import Env
from datetime import date


def save_rl_parameters_to_file():
    f = open("csvFiles/tryCSV1.csv", "a")

    w_target = 4
    w_col = -4.0  # original value
    w_smooth_v = -0.01
    w_smooth_w = -0.01
    w_smooth_acceleration = -0.006
    w_pref_v = 0.0
    w_pref_w = 0.0
    w_pref_dir = 0.02
    w_bubble = 0.0
    w_comfortDist = 0.01

    model_name = "modelHallway23stFeb23_2"

    today = date.today()
    creation_date = today.strftime("%d/%m/%Y")
    comment = "iterations= 5000 . agents=10, obstacles=3, batch size=64. _max_step = 1000 in Env.h file. My Hallway scenario. Training with calling our Revert() & RevertObs() in Env.cpp implemented with PBD.  no comfort distance; limiting velocity, alpha= 0.95; _time_step = 0.09 in Agent.h; used collision_dist_multiplier_obstacle = 1.0, float collision_dist_multiplier_agent = 1.0, max vel 1.3; Change in position of velocity limiting eqn; w_dist = (1+(fabs(cur_w - prev_w)/(2*PI))) * (1+(fabs(cur_w - prev_w)/(2*PI))) * 5; in getSmoothScore(). Modified getTargetScore() in Env.cpp. else if((dist > 0.0) && (curDist>PrevDist)){ score = (-0.25) * (dist); }"
    f.write("%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s\n" % (
        w_target, w_col, w_smooth_v, w_smooth_w, w_smooth_acceleration, w_pref_v, w_pref_w, w_pref_dir, w_bubble,
        w_comfortDist, model_name, creation_date, comment))
    f.close()


def make_env(seed=None):
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


def train(num_timesteps, path=None):
    from baselines.ppo1 import mlp_policy, pposgd_simple

    sess = U.make_session()
    sess.__enter__()

    def policy_fn(name, ob_space, ac_space):
        return mlp_policy.MlpPolicy(name=name, ob_space=ob_space, ac_space=ac_space,
                                    hid_size=64, num_hid_layers=3)

    env = make_env()
    print("prev path : ", path)

    pi = pposgd_simple.learn(env, policy_fn,
                             max_timesteps=num_timesteps,
                             timesteps_per_actorbatch=1024,
                             clip_param=0.2, entcoeff=0.0,
                             optim_epochs=10,
                             optim_stepsize=5e-5,
                             optim_batchsize=64,  # it was 64 by default
                             gamma=0.99,
                             lam=0.95,
                             schedule='linear',
                             # model_path=path
                             )
    env.env.plotSave()
    print("pi from run: ", pi)
    # pi.save()

    saver = tf.compat.v1.train.Saver(tf.compat.v1.all_variables())
    saver.save(sess, 'models/modelHallway23stFeb23_2')
    save_rl_parameters_to_file()

    return pi


def main():
    logger.configure()
    path_ = "models/modelHallway23stFeb23_2"
    train(num_timesteps=5120000, path=path_)


if __name__ == '__main__':
    main()
