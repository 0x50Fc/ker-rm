// pages/Location/index.js
/*
 * npm install -g ker.wx.compiler
 * ker package ker/main  ker/bin 1.0 
 */
Page({
  onShareAppMessage() {
    return {
      title: '文件api测试',
      path: 'page/API/pages/file-test/index'
    }
  },


  onLoad: function (options) {
   console.log("123ddge");
  },

  getFileInfo(e){
    console.log(getFileInfo);
    wx.choose
    wx.chooseImage({
      sourceType:["album"],
      success(e){
        console.log("file choose image res = ", res);
      }
    });


  }





 
})