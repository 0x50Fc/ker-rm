// pages/Location/index.js
/*
 * npm install -g ker.wx.compiler
 * ker package ker/main  ker/bin 1.0 
 */
Page({
  onShareAppMessage() {
    return {
      title: '录音测试',
      path: 'page/API/pages/bluetoooth-test/index'
    }
  },


  onLoad: function (options) {
   
  },

  getRecorderManager(e){
    var remanager = wx.getRecorderManager();
    remanager.star();
    console.log(remanager);
  }
 
})