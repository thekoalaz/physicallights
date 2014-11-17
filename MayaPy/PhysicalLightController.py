import maya.cmds as cmds
import pymel.core as pm

LIGHTS_NAMESPACE = "lights"

class PhysicalLightController(object):
    def __init__(self):
        print("Initializing controller.") 
        self.lights = {}
        self.lights[0] = LIGHTS_NAMESPACE + ":aiAreaLight_key"
        self.lights[1] = LIGHTS_NAMESPACE + ":aiAreaLight_fill"

    def translateLight(self, id, x, y, z):
        print("Translating " + self.lights[id] + "to" + str((x, y, z)))
        pm.general.setAttr(self.lights[id]+".translateX", x)
        pm.general.setAttr(self.lights[id]+".translateY", y)
        pm.general.setAttr(self.lights[id]+".translateZ", z)

    def rotateLight(self, id, x, y, z):
        print("Rotating " + self.lights[id] + "to" + str((x, y, z)))
        pm.general.setAttr(self.lights[id]+".rotateX", x)
        pm.general.setAttr(self.lights[id]+".rotateY", y)
        pm.general.setAttr(self.lights[id]+".rotateZ", z)

    def tester(self):
        self.translateLight(0, 0, 0, 0)
        self.rotateLight(0, 0, 0, 0)

ctrlr = PhysicalLightController()
ctrlr.tester()
