Page({
  onShareAppMessage() {
    return {
      title: '屏幕亮度',
      path: 'page/API/pages/screen-brightness/screen-brightness'
    }
  },

  data: {
    screenBrightness: 0
  },

  onLoad() {
    this._updateScreenBrightness()
  },

  changeBrightness(e) {
    // Number 类不可调用 等待实现后再用下面代码
    // const value = Number.parseFloat(
    //   (e.detail.value).toFixed(1)
    // )
    const value = e.detail.value;
    wx.setScreenBrightness({
      value,
      success: () => {
        this._updateScreenBrightness()
      }
    })
  },

  _updateScreenBrightness() {
    wx.getScreenBrightness({
      success: (res) => {
        this.setData({
          // screenBrightness: Number.parseFloat(
          //   res.value.toFixed(1)
          // )
          screenBrightness: res.value
        })
      },
      fail(err) {
        console.error(err)
      }
    })
  }
})
