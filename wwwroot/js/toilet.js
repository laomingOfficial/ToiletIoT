"use strict";

function timeago_zhTW(number, index) {
    return [
        ['剛剛', '片刻後'],
        ['%s 秒前', '%s 秒後'],
        ['1 分鐘前', '1 分鐘後'],
        ['%s 分鐘前', '%s 分鐘後'],
        ['1 小時前', '1 小時後'],
        ['%s 小時前', '%s 小時後'],
        ['1 天前', '1 天後'],
        ['%s 天前', '%s 天後'],
        ['1 週前', '1 週後'],
        ['%s 週前', '%s 週後'],
        ['1 個月前', '1 個月後'],
        ['%s 個月前', '%s 個月後'],
        ['1 年前', '1 年後'],
        ['%s 年前', '%s 年後'],
    ][index];
}
timeago.register("zh_TW", timeago_zhTW);

var app = new Vue({
    el: "#app",
    data() {
        return {
            deviceId: "UyAxQU7xoaK4LM7V3xd70U7K7RBfDWs2KxEL",

            toiletList: [],
            alarmSongList: [
                {
                    DisplayName: "鬼滅之刃 紅蓮華",
                    Value: "DemonSlayer"
                },
                {
                    DisplayName: "馬里奧 地底世界",
                    Value: "MarioUnderworld"
                }
            ],

            signalRconnection: null,
            isLoading: true
        }
    },
    created() {
        this.initSignalR();
    },
    methods: {
        initSignalR() {
            var _app = this;
            this.signalRconnection = new signalR.HubConnectionBuilder()
                .withUrl("/toiletHub")
                .withAutomaticReconnect([0, 1000, 5000,])
                .build();
            this.signalRconnection.on("ReceiveMQTT", function (message) {
                var data = JSON.parse(message);
                console.log(data);

                if (data.action == "notification/insert") {
                    if (data.notification.notification == "toiletStatus") {
                        var timestamp = data.notification.timestamp + "Z";
                        console.log(timestamp)
                        var param = data.notification.parameters;

                        var toiletArr = _app.toiletList.filter(x => x.iotId == param.iotId);
                        if (toiletArr.length <= 0) {
                            _app.addToilet(param.iotId, param.status, param.power, timestamp);
                        } else {
                            toiletArr[0].status = param.status;
                            toiletArr[0].batteryLevel = param.power;
                            toiletArr[0].lastUpdate = timestamp;
                        }

                        _app.$nextTick(() => {
                            timeago.render(document.querySelectorAll('.need_to_be_rendered'), 'zh_TW');
                        });
                    }
                } else if (data.action == "command/insert") {

                }
            });
            this.signalRconnection.start().then(function () {
                _app.loadData();
                _app.isLoading = false;
            }).catch(function (err) {
                return console.error(err.toString());
            });
        },
        addToilet(iotId, status, batteryLevel, lastUpdate) {
            this.toiletList.push({
                iotId: iotId,
                status: status,
                batteryLevel: batteryLevel,
                lastUpdate: lastUpdate,
                alarmSong: this.alarmSongList[0].Value
            });
        },
        loadData() {
            var obj = {
                "action": "command/insert",
                "deviceId": this.deviceId,
                "command": {
                    "command": "retrieveStatus",
                    "parameters": {
                        "iotId": "ALL"
                    },
                    "status": "to do",
                    "result": {}
                }
            };
            this.sendCommand(obj);
        },
        alertToilet(toiletIndex, alarmSongIndex) {
            var obj = {
                "action": "command/insert",
                "deviceId": this.deviceId,
                "command": {
                    "command": "beepAlarm",
                    "parameters": {
                        "iotId": this.toiletList[toiletIndex].iotId
                    },
                    "status": "to do",
                    "result": {
                        "beepSong": this.toiletList[toiletIndex].alarmSong
                    }
                }
            };
            this.sendCommand(obj);
        },
        sendCommand(obj) {
            this.signalRconnection.invoke("SendCommand", JSON.stringify(obj)).catch(function (err) {
                return console.error(err.toString());
            });
        }
    }
});
