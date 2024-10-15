import sys
import time
import os
import socket
import serial
import serial.tools.list_ports as sp
import struct
import csv
import string
import pandas as pd

from os import path
from PySide6 import QtCore
from PySide6.QtWidgets import QApplication, QMainWindow, QFileDialog, QHBoxLayout
from PySide6.QtCore import QFile, QAbstractTableModel
from PySide6.QtGui import *
from ui.ui_can import *
from ui.ui_main import *
from configparser import ConfigParser

from lib.cmd import *
from lib.cmd_can import *
from tab.info import *







class TabCAN(QWidget, Ui_CAN):
  def __init__(self, ui_main :Ui_MainWindow, cmd, config_item, syslog):
    super().__init__()
    self.setupUi(self)

    self.ui_main = ui_main
    self.bit_rate = {"100K":0, "125K":1, "250K":2, "500K":3, "1M":4, "2M":5, "4M":6, "5M":7}
    self.dlc_str = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "12", "16", "20", "24", "32", "48", "64"]
    self.can_tx_head = {"Sel":0, "ID":1, "Ext.ID":2, "FD":3, "BRS":4, "DLC":5, "Data":6}
    self.can_rx_head = {"Time":0, "ID":1, "Tx/Rx":2, "Type":3, "DLC":4, "Data":5}

    self.is_open = False
    self.cmd = cmd
    self.cmd_can = CmdCAN(cmd)    
    self.config_item = config_item
    self.log = syslog
    self.can_filter = CmdCANFilter()
    self.can_rx_count = 0

    self.audio_music_count = 0
    self.combo_open_rate_normal.clear()
    
    for item in self.bit_rate:
      if self.bit_rate[item] <= self.bit_rate["1M"]:
        self.combo_open_rate_normal.addItem(item)


    self.loadConfig()

    self.setClickedEvent(self.btn_open, self.btnOpen)
    self.setClickedEvent(self.btn_close, self.btnClose)


    # self.check_open_canfd.stateChanged.connect(self.btnUpdate)  
    # self.check_open_brs.stateChanged.connect(self.btnUpdate)  
    # print(list(main_info_tbl.keys()).index('dMain_RotationSet'))

    self.setClickedEvent(self.btn_spine_play, self.btnSpinePlay)
    self.setClickedEvent(self.btn_spine_pause, self.btnSpinePause)
    self.setClickedEvent(self.btn_spine_stop, self.btnSpineStop)
    self.setClickedEvent(self.btn_ptc_set, self.btnPtcSet)

    self.setClickedEvent(self.btn_reclining_up, self.btnRecliningUp)
    self.setClickedEvent(self.btn_reclining_down, self.btnRecliningDown)
    self.setClickedEvent(self.btn_reclining_stop, self.btnRecliningStop)

    self.setClickedEvent(self.btn_legrest_up, self.btnLegrestUp)
    self.setClickedEvent(self.btn_legrest_down, self.btnLegrestDown)
    self.setClickedEvent(self.btn_legrest_stop, self.btnLegrestStop)

    self.setClickedEvent(self.btn_heater_set_arm, self.btnHeaterSetArm)
    self.setClickedEvent(self.btn_heater_set_cush, self.btnHeaterSetCush)
    self.setClickedEvent(self.btn_heater_set_leg, self.btnHeaterSetLeg)

    self.setClickedEvent(self.btn_air_play, self.btnAirPlay)
    self.setClickedEvent(self.btn_air_pause, self.btnAirPause)

    self.setClickedEvent(self.btn_rmc_power_on, self.btnRmcPowerOn)
    self.setClickedEvent(self.btn_rmc_power_off, self.btnRmcPowerOff)

    self.setClickedEvent(self.btn_ext_dev_play, self.btnExtDevPlay)
    self.setClickedEvent(self.btn_ext_dev_pause, self.btnExtDevPause)
    self.setClickedEvent(self.btn_ext_dev_stop, self.btnExtDevStop)
    self.setClickedEvent(self.btn_ext_dev_temp_set, self.btnExtDevTempSet)

    
    self.setClickedEvent(self.btn_audio_ctrl_play, self.btnAudioCtrlPlay)
    self.setClickedEvent(self.btn_audio_ctrl_pause, self.btnAudioCtrlPause)
    self.setClickedEvent(self.btn_audio_ctrl_stop, self.btnAudioCtrlStop)


    self.setValueChangedEvent(self.comboBox_audio_reset, self.btnAudioCtrlReset)
    self.setValueChangedEvent(self.comboBox_audio_select_sound, self.btnAudioCtrlSelectSound)
    self.setValueChangedEvent(self.comboBox_audio_bt_auto_pair, self.btnAudioCtrlBtAutoPair)
    self.setValueChangedEvent(self.comboBox_audio_bt_pair, self.btnAudioCtrlBtPair)
    self.setValueChangedEvent(self.comboBox_audio_select_track, self.btnAudioCtrlMusicSelectSet)
    self.setValueChangedEvent(self.comboBox_audio_song_select, self.btnAudioCtrlSongSelectSet)
    
    self.setTextChangedEvent(self.lineEdit_audio_ctrl_music_volume, self.btnAudioCtrlMusicVolume)
    self.setTextChangedEvent(self.lineEdit_audio_ctrl_voice_volume, self.btnAudioCtrlVoiceVolume)

    vlabels = []
    for item in bldc_info_tbl:
      vlabels.append(item)
        
    self.table_bldc.setColumnCount(2)

    self.table_bldc.setRowCount(len(vlabels))
    self.table_bldc.setVerticalHeaderLabels(vlabels)
    self.table_bldc.resizeRowsToContents()


    vlabels = []
    for item in main_info_tbl:
      vlabels.append(item)

    self.comboBox_send_1.addItems(vlabels)
    self.comboBox_send_2.addItems(vlabels)
    self.comboBox_send_3.addItems(vlabels)
    self.comboBox_send_4.addItems(vlabels)

    # CAN화면에서 Send할 데이터 미리 던지는..
    #self.comboBox_send_1.setCurrentIndex(list(main_info_tbl.keys()).index('dMain_VerticalLeftPositionSet'))
    #self.comboBox_send_2.setCurrentIndex(list(main_info_tbl.keys()).index('dMain_VerticalLeftSpeedSet'))
    #self.comboBox_send_3.setCurrentIndex(list(main_info_tbl.keys()).index('dMain_RotationPositionSet'))
    #self.comboBox_send_4.setCurrentIndex(list(main_info_tbl.keys()).index('dMain_RotationSpeedSet'))
    

    # item = QTableWidgetItem("test")
    # self.table_bldc.setItem(2,0,item) 

    self.btn_tbl = [ 
      [self.btn_send_1, self.lineEdit_send_1, self.comboBox_send_1, self.label_1],
      [self.btn_send_2, self.lineEdit_send_2, self.comboBox_send_2, self.label_2],
      [self.btn_send_3, self.lineEdit_send_3, self.comboBox_send_3, self.label_3],
      [self.btn_send_4, self.lineEdit_send_4, self.comboBox_send_4, self.label_4],
      ]
    
    for item in self.btn_tbl:
      self.setClickedEventArg(item[0], self.btnSend, [item[1], item[2], item[3]])  
      item[2].currentTextChanged.connect(self.btnUpdate)


    # self.setClickedEventArg(self.btn_send_1, self.btnSend, [self.lineEdit_send_1, self.comboBox_send_1])  
    # self.setClickedEventArg(self.btn_send_2, self.btnSend, [self.lineEdit_send_2, self.comboBox_send_2])  
    # self.setClickedEventArg(self.btn_send_3, self.btnSend, [self.lineEdit_send_3, self.comboBox_send_3])  
    # self.setClickedEventArg(self.btn_send_4, self.btnSend, [self.lineEdit_send_4, self.comboBox_send_4])  

    self.btnUpdate()

    self.rx_size_data = 0
    self.rx_size_timer=  QtCore.QTimer(self)
    self.rx_size_timer.start(1000)
    self.rx_size_timer.timeout.connect(self.rxSizeTimerISR)


    self.comboBox_masg_list.addItems(massage_pattern_tbl)
    self.comboBox_spine_speed.addItems(massage_speed_tbl)
    self.comboBox_spine_strength.addItems(massage_strength_tbl)

    self.lineEdit_ptc_set.setText("0")
    self.lineEdit_heater_arm_temp_target.setText("0")
    self.lineEdit_heater_cush_temp_target.setText("0")
    self.lineEdit_heater_leg_temp_target.setText("0")
    
    self.comboBox_air_mode.addItems(air_pattern_tbl)
    self.comboBox_air_strength.addItems(massage_strength_tbl)

    self.comboBox_ext_dev_mode.addItems(ext_dev_massage_pattern_tbl)
    self.comboBox_ext_dev_strength.addItems(ext_dev_strength_tbl)

    self.comboBox_audio_select_sound.addItems(audio_select_sound_tbl)
    self.comboBox_audio_bt_auto_pair.addItems(audio_bt_auto_pair_tbl)
    self.comboBox_audio_bt_pair.addItems(audio_bt_auto_pair_tbl)
    self.comboBox_audio_reset.addItems(audio_bt_auto_pair_tbl)

    self.mp3_name = bytearray(16)

    self.comboBox_audio_select_track.addItem('Not')    # 0
    self.comboBox_audio_select_track.addItem('Back')   # 1
    self.comboBox_audio_select_track.addItem('Foward') # 2


  def rxSizeTimerISR(self):
    # if self.cmd_can.is_open == True:
    #   self.log.print("test")
    out_str = "RX SIZE : " + str(int(self.rx_size_data/1024)) + " KB/sec "
    out_str += str(int((self.rx_size_data * 100 / (500000/8)))) + "%"
    self.label_rx_size.setText(out_str)   

    self.rx_size_data = 0
    



  def setBldcTableData(self, index, data):
    item = QTableWidgetItem(str(data))
    item2Hex = QTableWidgetItem(hex(data))
    self.table_bldc.setItem(index, 0, item)
    self.table_bldc.setItem(index, 1, item2Hex)
  

  def __del__(self):
    return

  def tableAutoScroll(self):  
    if self.check_autoscroll.isChecked():
      QtCore.QTimer.singleShot(0, self.view_can_rx.scrollToBottom)

  def tableUpdate(self):  
    if self.tm.rowCount() < 5000:
      if self.table_timer.interval() > 100:
        self.table_timer.setInterval(100)
    else:
      if self.table_timer.interval() < 500:
        self.table_timer.setInterval(500)
    self.tm.updateRows()

  def setClickedEvent(self, event_dst, event_func):
    event_dst.clicked.connect(lambda: self.btnClicked(event_dst, event_func))   

  def setClickedEventArg(self, event_dst, event_func, func_arg):
    event_dst.clicked.connect(lambda: self.btnClickedArg(event_dst, event_func, func_arg))   
    
  '''
  콤보박스 값 변경시
  '''
  def setValueChangedEvent(self, event_dst, event_func):
    event_dst.activated.connect(lambda: self.btnClicked(event_dst, event_func))   

  '''
  lineEdit 값 변경시
  '''
  def setTextChangedEvent(self, event_dst, event_func):
    event_dst.textChanged.connect(lambda: self.btnClicked(event_dst, event_func))   

  def btnClicked(self, button, event_func):
    event_func()   
    self.btnUpdate()    

  def btnClickedArg(self, button, event_func, func_arg):
    event_func(func_arg)   
    self.btnUpdate()    


  def btnUpdate(self):
    
    if self.cmd.is_open == False:
      self.is_open = False

    if self.is_open == True:
      self.btn_open.setEnabled(False)
      self.btn_close.setEnabled(True)

      self.combo_open_rate_normal.setEnabled(False)
      self.combo_open_mode.setEnabled(False)
    else:
      self.btn_open.setEnabled(True)
      self.btn_close.setEnabled(False)

      self.combo_open_rate_normal.setEnabled(True)
      self.combo_open_mode.setEnabled(True)

    for item in self.btn_tbl:
      combo_box:QComboBox = item[2]
      label_:QLabel = item[3]

      try:
        if main_info_tbl[combo_box.currentText()][2] is not None:
          label_.setText(main_info_tbl[combo_box.currentText()][2])
      except:
        label_.setText(" ")

  def getTableCenterWidget(self, item):
    cell_widget = QWidget()
    layout = QHBoxLayout(cell_widget)
    layout.addWidget(item)
    layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
    layout.setContentsMargins(0, 0, 0, 0)
    cell_widget.setLayout(layout)    
    return cell_widget

  def sendMsg(self, id, bank, number, data):
    if self.cmd_can.is_open == False:
      print("Not Open")
      return

    can_msg = CmdCANPacket()
    can_msg.id = id     
    can_msg.frame = CAN_CLASSIC
    can_msg.dlc = 8
    can_msg.length = 8
    can_msg.id_type = CAN_STD

    #send_buf = pack(">BBHi", bank, 2, number, data)
    send_buf = pack(">BBHI", bank, 2, number, data)
    
    for item in send_buf:      
      print(hex(item))

    can_msg.data = send_buf

    self.cmd_can.send(can_msg)


  def btnRotatePos(self):
    value = self.lineEdit_rotate_pos.text()
    print(value)    
    self.sendMsg(3, 10, int(value))

  def btnRotateSpd(self):
    value = self.lineEdit_rotate_spd.text()
    print(value)

  def btnSend(self, func_arg):
    line_edit:QLineEdit = func_arg[0]
    combo_box:QComboBox = func_arg[1]

    bank   = main_info_tbl[combo_box.currentText()][0]
    number = main_info_tbl[combo_box.currentText()][1]
    value  = line_edit.text()
    out_str = combo_box.currentText() + " "
    out_str += str(bank) + " "
    out_str += str(number) + " "
    out_str += value
    print(out_str)
    self.sendMsg(0x202, bank, number, int(value))


  # def btnSend(self):
  #   row_pos = self.table_can_tx.rowCount()

  #   for i in range(row_pos):
  #     sel = self.table_can_tx.cellWidget(i, self.can_tx_head["Sel"]).layout().itemAt(0).widget().isChecked()
  #     if sel == False:
  #       continue

  #     fd = self.table_can_tx.cellWidget(i, self.can_tx_head["FD"]).layout().itemAt(0).widget().isChecked()
  #     fd_brs = self.table_can_tx.cellWidget(i, self.can_tx_head["BRS"]).layout().itemAt(0).widget().isChecked()
  #     ext_id = self.table_can_tx.cellWidget(i, self.can_tx_head["Ext.ID"]).layout().itemAt(0).widget().isChecked()
  #     dlc = self.table_can_tx.cellWidget(i, self.can_tx_head["DLC"]).layout().itemAt(0).widget().currentIndex()
  #     length = int(self.dlc_str[dlc])

  #     can_msg = CmdCANPacket()
  #     can_msg.id = int(self.table_can_tx.item(i, self.can_tx_head["ID"]).text(), 16)
      
  #     if fd == True and fd_brs == True:
  #       can_msg.frame = CAN_FD_BRS  
  #     elif fd == True and fd_brs == False:
  #       can_msg.frame = CAN_FD_NO_BRS
  #     else:
  #       can_msg.frame = CAN_CLASSIC

  #     can_msg.dlc = dlc
  #     can_msg.length = length
  #     if ext_id == True:
  #       can_msg.id_type = CAN_EXT
  #     else:
  #       can_msg.id_type = CAN_STD

  #     data_str = self.table_can_tx.item(i, self.can_tx_head["Data"]).text()
  #     data_bytes = bytes.fromhex(data_str)
  #     index = len(data_bytes)
  #     can_msg.data[:index] = data_bytes[:index]
  #     for i in range(index, length):
  #       can_msg.data[i] = 0x00

  #     self.cmd_can.send(can_msg)


  def is_hex(self, s):
     hex_digits = set(string.hexdigits)
     return all(c in hex_digits for c in s)

  def getCanDataLine(self, can_msg_line):
    line_list = can_msg_line.split(" ")
    out_line = ""
    for data in line_list:
      data = data.upper()
      if len(data) == 2 and self.is_hex(data):        
        out_line += data + " "

    return out_line

  def btnOpen(self):  
    if self.is_open == True:
      self.is_open = False
      self.btnUpdate()
      return

    if self.cmd.is_open:
      option = CmdCANOpen()
      option.mode = self.combo_open_mode.currentIndex()
      option.frame = CAN_CLASSIC
      option.baud = self.combo_open_rate_normal.currentIndex()

      err_code, resp = self.cmd_can.open(option)
      if err_code == OK:
        self.is_open = True
        self.saveConfig()
      self.is_open = True    
      self.can_rx_count = 0  
    else:
      self.log.println("Not Connected")
    self.btnUpdate()

    if self.is_open:
      err_code, resp = self.cmd_can.getFilter(self.can_filter)
      if err_code == OK:
        self.log.printLog("getFilter()")
      else:
        self.log.printLog("getFilter() Fail")

  def btnClose(self):  
    self.is_open = False
    if self.cmd.is_open:
      err_code, resp = self.cmd_can.close()
    else:
      self.log.println("Not Connected")
    self.btnUpdate()

  def btnSpinePlay(self):
    strength = self.comboBox_spine_strength.currentText()
    speed = self.comboBox_spine_speed.currentText()
    mode = str(self.comboBox_masg_list.currentIndex())
    if self.checkBox_scan.isChecked():
      scan_mode = "1"
    else:
      scan_mode = "2"

    self.sendMsgFromTable("dRMC_MassageStrengthSet", strength)
    self.sendMsgFromTable("dRMC_MassageSpeedSet", speed)
    self.sendMsgFromTable("dRMC_MassageModeSet", mode)
    self.sendMsgFromTable("dRMC_SCAN_ControlCmd", scan_mode)
    self.sendMsgFromTable("dRMC_MassagePlaySet", "2")

  def btnSpinePause(self):
    self.sendMsgFromTable("dRMC_MassagePlaySet", "1")

  def btnSpineStop(self):
    self.sendMsgFromTable("dRMC_MassagePlaySet", "5")

  def btnPtcSet(self):
    ptc_set = int(self.lineEdit_ptc_set.text())    
    self.sendMsgFromTable("dRMC_Temp_InPTC_Set", ptc_set)

  def btnRecliningUp(self):
    self.sendMsgFromTable("dRMC_Reclining_Set", "3")

  def btnRecliningDown(self):
    self.sendMsgFromTable("dRMC_Reclining_Set", "2")

  def btnRecliningStop(self):
    self.sendMsgFromTable("dRMC_Reclining_Set", "1")

  def btnLegrestUp(self):
    self.sendMsgFromTable("dRMC_Legrest_Set", "3")

  def btnLegrestDown(self):
    self.sendMsgFromTable("dRMC_Legrest_Set", "2")

  def btnLegrestStop(self):
    self.sendMsgFromTable("dRMC_Legrest_Set", "1")

  def btnHeaterSetArm(self):
    self.sendMsgFromTable("dRMC_Temp_InHeater_Set", self.lineEdit_heater_arm_temp_target.text())

  def btnHeaterSetCush(self):
    self.sendMsgFromTable("dRMC_Temp_SubHeater_Set", self.lineEdit_heater_cush_temp_target.text())

  def btnHeaterSetLeg(self):
    self.sendMsgFromTable("dRMC_Temp_LegHeater_Set", self.lineEdit_heater_leg_temp_target.text())


  def btnAirPlay(self):
    strength = self.comboBox_air_strength.currentText()
    mode = str(self.comboBox_air_mode.currentIndex())

    self.sendMsgFromTable("dRMC_SubStrengthSet", strength)
    self.sendMsgFromTable("dRMC_SubModeSet", mode)
    self.sendMsgFromTable("dRMC_SubPlaySet", "2")

  def btnAirPause(self):
    self.sendMsgFromTable("dRMC_SubPlaySet", "1")


  def btnRmcPowerOn(self):
    self.log.println("btnRmcPowerOn")
    self.sendMsgFromTable("dPower_RMC", "2")

  def btnRmcPowerOff(self):
    self.log.println("NbtnRmcPowerOff")
    self.sendMsgFromTable("dPower_RMC", "1")


  def btnAudioCtrlReset(self):
    value = self.comboBox_audio_reset.currentIndex()
    self.sendMsgFromTable("dRMC_Control_Reset_Set", str(value))

  def btnAudioCtrlSelectSound(self):
    value = self.comboBox_audio_select_sound.currentIndex()
    self.sendMsgFromTable("dRMC_Control_SDBT_Select_Set", str(value))

  def btnAudioCtrlBtAutoPair(self):
    value = self.comboBox_audio_bt_auto_pair.currentIndex()
    self.sendMsgFromTable("dRMC_Control_AutoPairing_Set", str(value))

  def btnAudioCtrlBtPair(self):
    value = self.comboBox_audio_bt_pair.currentIndex()
    self.sendMsgFromTable("dRMC_Control_Pariring_Set", str(value))

  def btnAudioCtrlMusicSelectSet(self):
    value = self.comboBox_audio_select_track.currentIndex()
    self.sendMsgFromTable("dRMC_TrackSelect_MusicSelect_Set", str(value))

  def btnAudioCtrlSongSelectSet(self):
    value = self.comboBox_audio_song_select.currentIndex()+1
    self.sendMsgFromTable("dRMC_TrackSelect_SongNumber_Set", str(value))

  def btnAudioCtrlMusicVolume(self):
    value = int(self.lineEdit_audio_ctrl_music_volume.text())
    self.sendMsgFromTable("dRMC_Control_MusicVolume_Set", str(value))

  def btnAudioCtrlVoiceVolume(self):
    value = int(self.lineEdit_audio_ctrl_voice_volume.text())
    self.sendMsgFromTable("dRMC_Control_VoiceVolume_Set", str(value))


  def btnAudioCtrlPlay(self):
    self.sendMsgFromTable("dRMC_Control_MusicControl_Set", "1")
  def btnAudioCtrlPause(self):
    self.sendMsgFromTable("dRMC_Control_MusicControl_Set", "2")
  def btnAudioCtrlStop(self):
    self.sendMsgFromTable("dRMC_Control_MusicControl_Set", "0")
    

  def btnExtDevPlay(self):
    strength = self.comboBox_ext_dev_strength.currentText()
    mode = str(self.comboBox_ext_dev_mode.currentIndex()+1)

    self.sendMsgFromTable("dRmc_Ext_Dev_PlayStength_set", strength)
    self.sendMsgFromTable("dRmc_Ext_Dev_PlayMode_set", mode)
    self.sendMsgFromTable("dRmc_Ext_Dev_PlayStatus_set", "1")

  def btnExtDevPause(self):
    self.sendMsgFromTable("dRmc_Ext_Dev_PlayStatus_set", "2")

  def btnExtDevStop(self):
    self.sendMsgFromTable("dRmc_Ext_Dev_PlayStatus_set", "0")

  def btnExtDevTempSet(self):
    temp_set = int(self.LineEdit_ext_dev_temp_set.text())    
    self.sendMsgFromTable("dRmc_Ext_Dev_Temp_Set", temp_set)

  def sendMsgFromTable(self, msg_name, value):
    if self.cmd.is_open == False:
      return
    bank   = main_info_tbl[msg_name][0]
    number = main_info_tbl[msg_name][1]
    self.sendMsg(0x202, bank, number, int(value))

  def loadConfig(self):        
    try:
      # if self.config_item['can_open_fd'] == "True":
      #   self.check_open_canfd.setChecked(True)
      # if self.config_item['can_open_brs'] == "True":
      #   self.check_open_brs.setChecked(True)

      self.combo_open_rate_normal.setCurrentIndex(int(self.config_item['can_open_rate_normal']))
      self.combo_open_mode.setCurrentIndex(int(self.config_item['can_open_mode']))
    except Exception as e:
      print(e)

  def saveConfig(self):  
    self.config_item['can_open_rate_normal'] = str(self.combo_open_rate_normal.currentIndex())
    self.config_item['can_open_mode'] = str(self.combo_open_mode.currentIndex())


  def addCanRxMsg(self, can_msg: CmdCANPacket, dir_str):    
    msg_item = []
    # msg_item.append(str(can_msg.timestamp))
    msg_item.append(str(int(round(time.time() * 1000))%0x10000))
    msg_item.append(str(hex(can_msg.id)))
    msg_item.append(dir_str)

    if can_msg.id_type == 0:
      frame_str = "STD "
    else:
      frame_str = "EXT "

    if can_msg.frame == CAN_CLASSIC:
      frame_str += ""
    elif can_msg.frame == CAN_FD_NO_BRS:
      frame_str += "FD"
    else:
      frame_str += "FD BRS"
    msg_item.append(frame_str)

    msg_item.append(self.dlc_str[can_msg.dlc])
    msg_item.append(can_msg.data[:can_msg.length].hex(" "))

    # self.tm.addDataOnly(msg_item)
    return

  def updateUiFromMsg(self, item, data):
    if item == 'dMain_PTC_Temp1_Now':
      self.lineEdit_ptc_get_1.setText(str(data)) 
    if item == 'dMain_PTC_Temp2_Now':
      self.lineEdit_ptc_get_2.setText(str(data)) 
    if item == 'dMain_PTC_Temp3_Now':
      self.lineEdit_ptc_get_3.setText(str(data)) 
    if item == 'dMain_PTC_Temp4_Now':
      self.lineEdit_ptc_get_4.setText(str(data)) 

    if item == 'dMain_PTC_PWM1_Set':
      self.lineEdit_ptc_pwm_1.setText(str(data)) 
    if item == 'dMain_PTC_PWM2_Set':
      self.lineEdit_ptc_pwm_2.setText(str(data)) 
    if item == 'dMain_PTC_PWM3_Set':
      self.lineEdit_ptc_pwm_3.setText(str(data)) 
    if item == 'dMain_PTC_PWM4_Set':
      self.lineEdit_ptc_pwm_4.setText(str(data)) 
    
    if item == 'dMain_Reclining_CurrentAngle':
      self.lineEdit_reclining_get_angle.setText(str(data))
    if item == 'dMain_Reclining_State':
      self.lineEdit_reclining_get_state.setText(str(data))

    if item == 'dMain_Legrest_State':
      self.lineEdit_legrest_state.setText(str(data))


    if item == 'dMain_InHeater_Temp_Now':
      self.lineEdit_heater_arm_l_temp_current.setText(str(data))
    if item == 'dMain_InHeater_Current_Now':
      self.lineEdit_heater_arm_l_current.setText(str(data))
    if item == 'dMain_InHeater_Set':
      self.lineEdit_heater_arm_l_set.setText(str(data))

    if item == 'dMain_InHeater_R_Temp_Now':
      self.lineEdit_heater_arm_r_temp_current.setText(str(data))
    if item == 'dMain_InHeater_R_Current_Now':
      self.lineEdit_heater_arm_r_current.setText(str(data))
    if item == 'dMain_InHeater_R_Set':
      self.lineEdit_heater_arm_r_set.setText(str(data))

    if item == 'dMain_SubHeater_Temp_Now':
      self.lineEdit_heater_cush_temp_current.setText(str(data))
    if item == 'dMain_SubHeater_Current_Now':
      self.lineEdit_heater_cush_current.setText(str(data))
    if item == 'dMain_SubHeater_Set':
      self.lineEdit_heater_cush_set.setText(str(data))

    if item == 'dMain_LegHeater_Temp_Now':
      self.lineEdit_heater_leg_temp_current.setText(str(data))
    if item == 'dMain_LegHeater_Current_Now':
      self.lineEdit_heater_leg_current.setText(str(data))
    if item == 'dMain_LegHeater_Set':
      self.lineEdit_heater_leg_set.setText(str(data))


    room_status_edit = [self.lineEdit_room_status_1,
                        self.lineEdit_room_status_2,
                        self.lineEdit_room_status_3,
                        self.lineEdit_room_status_4,
                        self.lineEdit_room_status_5,
                        self.lineEdit_room_status_6]
    room_status_str = ["dSub_Room1State",
                       "dSub_Room2State",
                       "dSub_Room3State",
                       "dSub_Room4State",
                       "dSub_Room5State",
                       "dSub_Room6State"]
    
    for idx, item_str in enumerate(room_status_str):
      if item == item_str:
        room_status_edit[idx].setText(self.getValveStatus(data))

    room_set_edit = [self.lineEdit_room_set_1,
                     self.lineEdit_room_set_2,
                     self.lineEdit_room_set_3,
                     self.lineEdit_room_set_4,
                     self.lineEdit_room_set_5,
                     self.lineEdit_room_set_6]
    room_set_str = ["dMain_Room1Set",
                    "dMain_Room2Set",
                    "dMain_Room3Set",
                    "dMain_Room4Set",
                    "dMain_Room5Set",
                    "dMain_Room6Set"]

    for idx, item_str in enumerate(room_set_str):
      if item == item_str:
        room_set_edit[idx].setText(self.getValveStatus(data))

    if item == 'dSub_SolValveVoltageState':
      out_str = "None"
      if data == 0:
        out_str = "OFF"
      if data == 1:
        out_str = "13V"
      if data == 2:
        out_str = "24V"
      self.lineEdit_solvalve_vol_status.setText(out_str)
    
    if item == 'dSub_AirPumpState':
      if data == 0:
        self.lineEdit_airpump_state.setText("OFF")
      else:
        self.lineEdit_airpump_state.setText("ON")

    if item == 'dMain_AirPumpDutySet':
      self.lineEdit_airpump_duty.setText(str(data))

    if item == 'dMain_AirPumpDutyType':
      self.lineEdit_airpump_duty_type.setText(str(data))

    if item == 'dSub_MassageState':
      self.lineEdit_airmassage_state.setText(str(data))

    if item == 'dSub_AirFittingState':
      self.lineEdit_air_fit.setText(str(data))

    if item == 'dSub_PressureSensorValue':
      self.lineEdit_air_pressure.setText(str(data))

    if item == 'dMain_TxMP3Name1':      
      self.mp3_name[0:4] = data.to_bytes(4, byteorder='big')
      self.lineEdit_audio_name.setText(self.mp3_name.decode('utf-8'))

    if item == 'dMain_TxMP3Name2':      
      self.mp3_name[4:4+4] = data.to_bytes(4, byteorder='big')
      self.lineEdit_audio_name.setText(self.mp3_name.decode('utf-8'))

    if item == 'dMain_TxMP3Name3':      
      self.mp3_name[8:8+4] = data.to_bytes(4, byteorder='big')
      self.lineEdit_audio_name.setText(self.mp3_name.decode('utf-8'))

    if item == 'dMain_TxMP3Name4':      
      self.mp3_name[12:12+4] = data.to_bytes(4, byteorder='big')
      self.lineEdit_audio_name.setText(self.mp3_name.decode('utf-8'))    
    
    if item == 'dMP3_RxMP3Control':
      data = data & 0xFFFFFFFF
      # Convert to bytes
      bytes_data =  data.to_bytes(4, byteorder='big')
      #bytes_data = struct.pack('>I', data)
      self.lineEdit_audio_reset.setText(str((bytes_data[0] >> 7) & 0x01))
      self.lineEdit_audio_sdcard.setText(str((bytes_data[0] >> 6) & 0x01))
      self.lineEdit_audio_bt_pair.setText(str((bytes_data[0] >> 5) & 0x01))
      self.lineEdit_audio_bt_auto_pair.setText(str((bytes_data[0] >> 4) & 0x01))
      if (bytes_data[0] & 0x01) == 0x01:
        self.lineEdit_audio_sound_select.setText('BT')
      else:
        self.lineEdit_audio_sound_select.setText('SD')
      
      val = (bytes_data[1] >> 6) & 0x03
      state = ''
      if val == 0:
        state = 'OFF'
      elif val == 1:
        state = 'ON'
      elif val == 2:
        state = 'ERR'
      self.lineEdit_audio_voice_recg_state.setText(state)
      
      val = (bytes_data[1] >> 4) & 0x03
      state = ''
      if val == 0:
        state = 'OFF'
      elif val == 1:
        state = 'ON'
      elif val == 2:
        state = 'ERR'
      self.lineEdit_audio_mp3_pwr_state.setText(state)
      self.lineEdit_audio_sd_loop_setting.setText(str((bytes_data[1] >> 2) & 0x03))
      self.lineEdit_audio_sd_play_mode.setText(str((bytes_data[1] >> 1) & 0x01))
      self.lineEdit_audio_bt_connect_state.setText(str((bytes_data[1] >> 0) & 0x01))

      self.lineEdit_audio_music_volume.setText(str(bytes_data[2]))
      self.lineEdit_audio_voice_volume.setText(str(bytes_data[3]))

    if item == 'dMP3_RxMP3Quantity1':
      bytes_data =  data.to_bytes(4, byteorder='big')
      if self.audio_music_count != (bytes_data[0] * 256 + bytes_data[1]):
        self.audio_music_count = (bytes_data[0] * 256 + bytes_data[1])
        self.comboBox_audio_song_select.clear()
        for i in range(self.audio_music_count):
          self.comboBox_audio_song_select.addItem(str(i+1))
      self.lineEdit_audio_music_count.setText(str(self.audio_music_count))
      
      # voice count
      #self.lineEdit_audio_.setText(str(bytes_data[2] * 256 + bytes_data[3]))
    if item == 'dMP3_RxMP3PlayState':
      bytes_data =  data.to_bytes(4, byteorder='big')
      #state = str(f'{bytes_data[0]&0x0F}.{bytes_data[1]&0x0F}.{bytes_data[2]&0x0F}.{bytes_data[3]&0x0F}.{(bytes_data[3]>>4)&0x0F}')
      track1_state = bytes_data[0]&0x0F
      track2_state = bytes_data[1]&0x0F
      track3_state = bytes_data[2]&0x0F      
      bt_play_state = (bytes_data[3]>>4)&0x0F
      sd_play_state = bytes_data[3]&0x0F
      self.lineEdit_audio_play_state.setText(f'{track1_state}.{track2_state}.{track3_state}')

      self.lineEdit_audio_play_state_bt.setText(self.getPlayStatus(bt_play_state))
      self.lineEdit_audio_play_state_sd.setText(self.getPlayStatus(sd_play_state))
    if item == 'dMP3_RxMP3VoiceVersion':
      bytes_data =  data.to_bytes(4, byteorder='big')
      num1, num2 = struct.unpack('>HH', bytes_data)      
      ver = str(f'{num1>>9}.{num2>>9}.{num2&0x1FF}')
      self.lineEdit_audio_voice_version.setText(ver)
      

    if item == 'dMP3_RxMP3MP3Version':
      bytes_data =  data.to_bytes(4, byteorder='big')
      num1, num2 = struct.unpack('>HH', bytes_data)      
      ver = str(f'{num1>>9}.{num2>>9}.{num2&0x1FF}')
      self.lineEdit_audio_mp3_version.setText(ver)

    if item == 'dMP3_RxMP3SDVersion':
      bytes_data =  data.to_bytes(4, byteorder='big')
      num1, num2 = struct.unpack('>HH', bytes_data)      
      ver = str(f'{num1>>9}.{num2>>9}.{num2&0x1FF}')
      self.lineEdit_audio_sd_version.setText(ver)

    if item == 'dMP3_RxMP3MusicTrackSelect':
      bytes_data =  data.to_bytes(4, byteorder='big')
      val = (bytes_data[1] << 16 | bytes_data[2] << 8 | bytes_data[3])
      self.lineEdit_audio_music_track_sel.setText(str(val))

      val = bytes_data[0] & 0x0F
      eq = 'None'
      if val == 0:
        eq = 'Nor'
      elif val == 1:
        eq = 'Clas'
      elif val == 2:
        eq = 'Rock'
      elif val == 3:
        eq = 'Jazz'
      elif val == 4:
        eq = 'Pop'
      elif val == 5:
        eq = 'Vocal'
      elif val == 6:
        eq = 'Bass'
      self.lineEdit_audio_eq.setText(eq)
    
    if item == 'dMP3_RxMP3SelectTrackTime':
      bytes_data =  data.to_bytes(4, byteorder='big')
      track_time = '{0:02d}:{1:02d}:{2:02d}/{3:02d}:{4:02d}:{5:02d}'.format((bytes_data[2]>>4), ((bytes_data[2]&0xF)<<2)|(bytes_data[3]>>6), bytes_data[3]&0x3F, (bytes_data[0]>>4), ((bytes_data[0]&0xF)<<2)|(bytes_data[1]>>6), bytes_data[1]&0x3F)
      
      self.lineEdit_audio_select_track_tm.setText(track_time)

    if item == 'dExtDev_Temp_Now':
      self.lineEdit_ext_dev_cur_temp.setText(str(data))    
    if item == 'dExtDev_PlayStatus':
      out_str = "None"
      if data == 0:
        out_str = "OFF"
      if data == 1:
        out_str = "PLAY"
      if data == 2:
        out_str = "PAUSE" # Not used
      if data == 3:
        out_str = "STOP"
      self.lineEdit_ext_dev_status.setText(out_str)    

  def getPlayStatus(self, data):
    out_str = "None"
    if data == 2:
      out_str = "Pause"
    if data == 1:
      out_str = "Play"
    if data == 0:
      out_str = "Stop"
    return out_str
  
  def getValveStatus(self, data):
    out_str = "None"
    if data == 4:
      out_str = "Drain"
    if data == 3:
      out_str = "Direct"
    if data == 2:
      out_str = "Push"
    if data == 1:
      out_str = "Hold"
    return out_str

  def updateBldcMsg(self, can_msg : CmdCANPacket):

    bank = int(can_msg.data[0])
    number = int(can_msg.data[2]*256 + can_msg.data[3])
    data = (can_msg.data[4] << 24) | (can_msg.data[5]<<16) | (can_msg.data[6]<<8) | can_msg.data[7]
    index = 0
    for item in bldc_info_tbl:
      if bldc_info_tbl[item][0] == bank and bldc_info_tbl[item][1] == number:
        self.setBldcTableData(index, data)
        self.updateUiFromMsg(item, data)

      index += 1    
    
      

  def receiveCanMsg(self, packet: CmdPacket):
    can_msg = CmdCANPacket()
    can_msg.setCmdPacket(packet)
    # self.addCanRxMsg(can_msg, "rx")
    # print(can_msg)
    # print(hex(can_msg.id))
    # if can_msg.id == 0x103:      
    self.updateBldcMsg(can_msg)

    self.can_rx_count += 1
    self.can_rx_count %= 1000
    self.label_rx_cnt.setText("RX CNT : " + str(self.can_rx_count))    

    self.rx_size_data += can_msg.length

    return
    
    
  def rxdEvent(self, packet: CmdPacket):
    if packet.cmd == self.cmd_can.CMD_CAN_DATA:
      if self.is_open == True:
        self.receiveCanMsg(packet)
        