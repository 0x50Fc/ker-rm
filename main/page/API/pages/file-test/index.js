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
    console.log("getFileInfo");
    wx.chooseImage({
      sourceType:["camera"],
      success(res){
        console.log("file choose image res = ", res);
        wx.getFileInfo({
          filePath:res.tempFilePaths[0],
          success(res){
            console.log("get file info res = ", res);
          }
        });
      }
    });
  },

  saveFile(e){
    console.log("save file");
    wx.chooseImage({
      sourceType:["camera"],
      success(res){
        console.log("file choose image res = ", res);

        wx.saveFile({
          tempFilePath:res.tempFilePaths[0],
          success(res){
            console.log("saveFile success = %@", res);
          },
          fail(res){
            console.log("saveFile fail = %@", res);
          },
          complete(res){
            console.log("saveFile complete = %@", res);
          }
        });
      }
    });
  },

  getSavedFileList(e){
    wx.getSavedFileList({
      success(res){
        console.log("getSavedFileList success = %@", res);
      },
      fail(res){
        console.log("getSavedFileList fail = %@", res);
      },
      complete(res){
        console.log("getSavedFileList complete = %@", res);
      }
    });

    "/Users/zuowu/Library/Developer/CoreSimulator/Devices/8E1F4618-B0E2-4422-A0EE-3305FE814786/data/Containers/Data/Application/34B5A762-770A-415D-9914-E358226888B5/Documents/store/KerWXObjectChooseImage.png"
  },

  getSavedFileInfo(e){
    var path = "/Users/zuowu/Library/Developer/CoreSimulator/Devices/8E1F4618-B0E2-4422-A0EE-3305FE814786/data/Containers/Data/Application/34B5A762-770A-415D-9914-E358226888B5/Documents/store/KerWXObjectChooseImage.png";
    wx.getSavedFileInfo({
      filePath:path,
      success(res){
        console.log("getSavedFileInfo success = %@", res);
      },
      fail(res){
        console.log("getSavedFileInfo fail = %@", res);
      },
      complete(res){
        console.log("getSavedFileInfo complete = %@", res);
      }
    });
  }





 
})