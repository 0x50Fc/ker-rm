
const app = getApp()

Page({

  data: {
    items : [
        {
            id : 'action1',
            title : 'Action1'
        },
        {
            id : 'action2',
            title : 'Action2'
        },
        {
            id : 'action3',
            title : 'Action3'
        }
    ]
  },

  onLoad: function () {
  },

  doTapAction : function(event) {
      console.log(event);
  }

})

