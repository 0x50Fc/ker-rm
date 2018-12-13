Page({
  onShareAppMessage() {
    return {
      title: '新增联系人',
      path: 'page/API/pages/add-contact/add-contact'
    }
  },

  submit(e) {
    const formData = e.detail.value
    wx.addPhoneContact({
      ...formData,
      success(res) {
        wx.showToast({
          title: '联系人创建成功'
        })
        console.log("addPhoneContact success res = ", res);
      },
      fail(res) {
        wx.showToast({
          title: '联系人创建失败'
        })

        console.log("addPhoneContact fail res = ", res);
      },

      complete(res){
        console.log("addPhoneContact complete res = ", res);
      }
    })
  }
})
