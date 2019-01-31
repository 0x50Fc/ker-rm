// pages/Location/index.js
/*
 * npm install -g ker.wx.compiler
 * ker package ker/main  ker/bin 1.0 
 */
Page({
  onShareAppMessage() {
    return {
      title: 'image api测试',
      path: 'page/API/pages/image-test/index'
    }
  },


  onLoad: function (options) {
   console.log("image 123ddge");
  },

  
  getImageInfo(e){
    wx.getImageInfo({
      src: "http://pic44.photophoto.cn/20170719/0012024451690249_b.jpg",
      success(res) {
        console.log("getImageInfo success res = ", res);
      },
      fail(res) {
        console.log("getImageInfo fail res = ", res);
      },
      complete(res) {
        console.log("getImageInfo complete res = ", res);
      }
    })
  },

  saveImageToPhotosAlbum(e){

    wx.chooseImage({
      success(res) {
        console.log("choose image res = ", res);
        wx.saveImageToPhotosAlbum({
          filePath: res.tempFilePaths[0],
          success(res){
            console.log("saveImageToPhotosAlbum success res = ", res);
          },
          fail(res) {
            console.log("saveImageToPhotosAlbum fail res = ", res);
          },
          complete(res) {
            console.log("saveImageToPhotosAlbum complete res = ", res);
          }

        })
      }
    })
  }





 
})