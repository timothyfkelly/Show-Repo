
#external libraries
import maya.cmds as cmds
import maya.openmaya as om
import maya.openmayaUI as omUI 
from Qt import QtCore, QtGui, QtWidgets
from pyside2.QtUiTools import QUiLoader
from shiboken2 import wrapInstance
import os, json, math

CWD= os.path.dirname(os.path.realpath(__file__) )

def main_maya_window():
  maya_window_ptr=omUI.MQtUtil.mainWindow()
  return wrapInstance(long(maya_window_ptr), QtWidgets.QWidget)

class Form(QtWidgets.QDialog):
  def __init__(self,parent=None):
    super(Form, self).__init__(parent=main_maya_window() )
    self.title = “node manager”
    self.setwindowtitle(self.title)
    self.__init_ui__()
    self.__init_signals__()

def deleteUI(self, name):
  if cmds.window(name, exists=True) :
    cmds.deleteUI(name)

def__init_ui__(self):
  loader = QUiloader()
  validator= QtGui.QDoubleValidator()
  self.ui = loader.load({}/node_manager.ui”.format(CWD), None)

  self.ui.reverseCheck.setVisible(False)
  self.ui.title = ‘Node Manager’
  self.deleteUI(self.title)
  self.setObjectName(self.title)
  self.__callbacks__()

  self.ui.remapInMax.setValidator(validator)
