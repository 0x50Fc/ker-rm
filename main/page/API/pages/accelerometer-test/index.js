// pages/Location/index.js
Page({
  onShareAppMessage() {
    return {
      title: '监听重力感应数据',
      path: 'page/API/pages/accelerometer-test/index'
    }
  },


  onLoad: function (options) {
    console.log("accelerometer-test on load");
    wx.onAccelerometerChange(function (res) {
      console.log("onAccelerometerChange res = ", res);

    })
  },


  btnStartAccelerometer(res){
    console.log("btnStartAccelerometer");
    wx.startAccelerometer({
      interval: 'game',
      success(res){
        console.log("StartAccelerometer success res = ", res);
      },
      fail(res){
        console.log("StartAccelerometer fail res = ", res);
      },
      complete(res){
        console.log("StartAccelerometer complete res = ", res);
      }
    });
  },

  btnStopAccelerometer(res){
    console.log("btnStopAccelerometer");
    wx.stopAccelerometer({
      success(res){
        console.log("stopAcceleromete success res = ", res);
      },
      fail(res){
        console.log("stopAcceleromete fail res = ", res);
      },
      complete(res){
        console.log("stopAcceleromete complete res = ", res);
      }
    });
  }
})