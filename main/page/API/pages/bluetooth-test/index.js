// pages/Location/index.js
/*
 * npm install -g ker.wx.compiler
 * ker package ker/main  ker/bin 1.0 
 */
Page({
  onShareAppMessage() {
    return {
      title: '蓝牙api测试',
      path: 'page/API/pages/bluetoooth-test/index'
    }
  },


  onLoad: function (options) {
    //监听蓝牙状态改变
    wx.onBluetoothAdapterStateChange(function (res) {
      console.log('onBluetoothAdapterStateChange res = ', res);
    });



    //搜索到蓝牙
    wx.onBluetoothDeviceFound(function(res) {
      console.log('onBluetoothDeviceFound res = ', res);
    });
  },

  //打开蓝牙
  openBluetoothAdapter(e){
    wx.openBluetoothAdapter({
      success(res){
        console.log("success openBluetoothAdapter res = ", res);
      },
      fail(res){
        console.log("fail openBluetoothAdapter res = ", res);
      },
      complete(res){
        console.log("complete openBluetoothAdapter res = ", res);
      }
    });
  },

  //关闭蓝牙
  closeBluetoothAdapter(e){
    wx.closeBluetoothAdapter({
      success(res){
        console.log("success closeBluetoothAdapter res = ", res);
      },
      fail(res){
        console.log("fail closeBluetoothAdapter res = ", res);
      },
      complete(res){
        console.log("complete closeBluetoothAdapter res = ", res);
      }
    });
  },

  //获取蓝牙状态
  getBluetoothAdapterState(e){
    wx.getBluetoothAdapterState({
      success(res){
        console.log("success getBluetoothAdapterState res = ", res);
      },
      fail(res){
        console.log("fail getBluetoothAdapterState res = ", res);
      },
      complete(res){
        console.log("complete getBluetoothAdapterState res = ", res);
      }
    });
  },

  //开始扫描
  startBluetoothDevicesDiscovery(res){
    wx.startBluetoothDevicesDiscovery({
      success(res){
        console.log("success startBluetoothDevicesDiscovery res = ", res);
      },
      fail(res){
        console.log("fail startBluetoothDevicesDiscovery res = ", res);
      },
      complete(res){
        console.log("fail startBluetoothDevicesDiscovery res = ", res);
      }
    });
  },

  //结束扫描
  stopBluetoothDevicesDiscovery(res){
    wx.stopBluetoothDevicesDiscovery({
      success(res){
        console.log("success stopBluetoothDevicesDiscovery res = ", res);
      },
      fail(res){
        console.log("fail stopBluetoothDevicesDiscovery res = ", res);
      },
      complete(res){
        console.log("fail stopBluetoothDevicesDiscovery res = ", res);
      }
    });
  },


  getBluetoothDevices(res){
    wx.getBluetoothDevices({
      success(res){
        console.log("success getBluetoothDevices res = ", res);
      },
      fail(res){
        console.log("fail getBluetoothDevices res = ", res);
      },
      complete(res){
        console.log("fail getBluetoothDevices res = ", res);
      }
    });
  }



 
})