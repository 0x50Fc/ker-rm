const util = require('../../../../util/util.js')

const formatLocation = util.formatLocation

Page({
  onShareAppMessage() {
    return {
      title: '获取位置',
      path: 'page/API/pages/get-location/get-location'
    }
  },

  data: {
    hasLocation: false,
  },
  getLocation() {
    const that = this
    wx.getLocation({
      success(res) {
        
        console.log("getLocation success res = ",res);
        that.setData({
          hasLocation: true,
          location: formatLocation(res.longitude, res.latitude)
        })
      },
      complete(res){
        console.log("getLocation complete res = ", res);
      },
      fail(res){
        console.log("getLocation fail res = ", res);
      }
    })
  },
  clear() {
    this.setData({
      hasLocation: false
    })
  }
})
