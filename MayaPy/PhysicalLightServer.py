import maya.cmds as cmds
import pymel.core as pm

class PhysicalLightServer(object):
    """description of class"""
    def __init__(self):
        pm.general.commandPort()

    def close(self):
        pm.general.commandPort(cl=True)
