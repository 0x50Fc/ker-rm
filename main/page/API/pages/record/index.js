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
    remanager = {};
    console.log("remanager = ", typeof remanager);
    remanager = wx.getRecorderManager();
    console.log("remanager = ", typeof remanager);
    remanager.start({
      duration:1111
    });

    
  }
 
})