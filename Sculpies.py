
-------------------------------------------------------------
#CONT#

def coloursJson(self):
  COLOR_PATH =“{}index_data.json”.format(JSON_FOLDER)
  with open(COLOR_PATH) as f:
    mayaColourDict = json.load(f)
    return mayaColourDict

def setJson(self):
  setlist={}
  characterName=self.getCharacterNames()
  for item in characterName:
    try:
      with open(“{}{}.json”.format(JSON_FOLDER, item), ‘r’ ) as f:
      setList[item] = json.load(f)
      except (IOError, ValueError) as error:
          print “no character data found”
      blanktemplate={“FACE_NOSE”,”FACE_EYES”, “FACE_MOUTH”,”ARM_L”, “ARM_R”, “LEG_L”, “LEG_R”}
      with open(“{}{}.json”.format(JSON_FOLDER, item), “w”) as outFile:
      json.dump(blankTemplate, outFile)
      setList[item] = blankTemplate
      return setlist, characterName

def populateList(self, rowsToMake, table):
  print “\nPopulating list - Sculpies”
  table.clearContents()
  table.setRowCount(0)
  table=self.fillUI(table)
  return table

def populateListLoc(self, rowsToMake, table, locators):
  print “\nPopulating list- locators”
  table.clearContents()
  table.setRowCount(0)
  table = self.fillUIloc(table, rowsToMake, locators)
  return table

def populateListSet(self, rowsToMake, table, sets):
  print “\nPopulating list- locators”
  table.clearContents()
  table.setRowCount(0)
  table = self.fillUIloc(table, rowsToMake, locators)
  return table
