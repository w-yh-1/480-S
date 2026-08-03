import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
//import com.external 1.0
import QtQml.Models 2.15
import QtQuick.Layouts 1.15
import Qt.labs.folderlistmodel 2.12
import QtQuick.VirtualKeyboard 2.15
import QtBluetooth 5.15

import IRSDK 1.0

import "."

Window {
    id: window

    width: 1920
    height: 1080
    color: "#010203"
    visible: true
    visibility: Window.FullScreen
    function addItem(item) {
        contentModel.append({"lineText": item, "photographed": false});
    }

    function updateModeText() {
        var mode = ebutton.getmoshi();
        var lang = ebutton.getyuyan();
        switch(mode) {
        case 0:
            if(lang === 0) testtr3.text = "模式：可见光";
            else if(lang === 1) testtr3.text = "Mode：Visible";
            else if(lang === 4) testtr3.text = "パターン：VIS";
            else if(lang === 5) testtr3.text = "Modalità：visibile";
            else testtr3.text = "Режим：RGB";
            break;
        case 1:
            if(lang === 0) testtr3.text = "模式：融合";
            else if(lang === 1) testtr3.text = "Mode：Fusion";
            else if(lang === 4) testtr3.text = "パターン：UV＋VIS";
            else if(lang === 5) testtr3.text = "Modalità：Fusion";
            else testtr3.text = "Режим：слияние";
            break;
        case 2:
            if(lang === 0) testtr3.text = "模式：紫外";
            else if(lang === 1) testtr3.text = "Mode：UV";
            else if(lang === 4) testtr3.text = "パターン：UV";
            else if(lang === 5) testtr3.text = "Modalità：UV";
            else testtr3.text = "Режим：УФ";
            break;
        case 3:
            if(lang === 0) testtr3.text = "模式：红外";
            else if(lang === 1) testtr3.text = "Mode：IR";
            else if(lang === 4) testtr3.text = "パターン：IR";
            else if(lang === 5) testtr3.text = "Modalità：IR";
            else testtr3.text = "Режим：ИК";
            break;
        case 4:
            if(lang === 0) testtr3.text = "模式：融合";
            else if(lang === 1) testtr3.text = "Mode：Fusion";
            else if(lang === 4) testtr3.text = "パターン：IR＋UV";
            else if(lang === 5) testtr3.text = "Modalità：Fusion";
            else testtr3.text = "Режим：слияние";
            break;
        }
    }

    Window {
        width: 1920
        height: 1080
        visible: false
        title: "蓝牙功能"

        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 16

            // ========== 搜索状态提示 ==========
            Text {
                text: "✅ 蓝牙搜索完成"
                color: "#2E8B57"
                font.pixelSize: 18
                font.bold: true
            }

            // ========== 连接成功提示 ==========
            Text {
                text: "🔗 已成功连接vivo"
                color: "#0066CC"
                font.pixelSize: 18
                font.bold: true
            }

            // ========== 设备数量 ==========
            Text {
                text: "当前搜索到蓝牙设备：4 个"
                font.pixelSize: 16
                color: "#333"
            }

            // ========== 设备列表标题 ==========
            Text {
                text: "蓝牙设备列表："
                font.pixelSize: 16
                font.bold: true
            }

            // ========== 假数据列表 ==========
            ListView {
                height: 320
                width: parent.width
                model: [
                    {"name":"vivo","addr":"48:8A:E8:75:B1:8F"},
                    {"name":"客厅的小米电视","addr":"65:C3:B2:3D:73:31"},
                    {"name":"MC0F88E4","addr":"43:39:E5:6B:F6:22"},
                    {"name":"N/A","addr":"63:27:98:A2:D7:F1"}
                ]

                delegate: Rectangle {
                    width: parent.width
                    height: 60
                    color: index % 2 === 0 ? "#F5F5F5" : "#FFFFFF"
                    radius: 6

                    Column {
                        anchors.centerIn: parent
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        spacing: 4

                        Text {
                            text: "📶 " + modelData.name
                            font.pixelSize: 16
                            color: "#222"
                        }

                        Text {
                            text: "MAC: " + modelData.addr
                            font.pixelSize: 12
                            color: "#666"
                        }
                    }
                }
            }

            // ========== 底部按钮（仅展示） ==========
            Button {
                text: "重新搜索"
                font.pixelSize: 14
                background: Rectangle { color: "#E0F0FF"; radius: 6 }
            }
        }
    }

    Image {
        id: logo
        objectName: "logo"
        x:0
        y:30
        visible: false
        width: 250 // 右半部分宽度
        height: 70
        source: "file:///opt/logo_e.png" // 初始为空
        fillMode: Image.PreserveAspectFit // 保持图片纵横比
    }

    // UV 范围调试框（绿色边框）
    property bool showUvDebugRect: true  // 设为 false 可关闭调试框
    Rectangle {
        id: uvDebugRect
        visible: false  // 预览不显示，仅在拍照时由 E_button.cpp 绘制到照片上
        x: ebutton.getIruvDstX()
        y: ebutton.getIruvDstY()
        width: ebutton.getIruvDstWidth()
        height: ebutton.getIruvDstHeight()
        color: "transparent"
        border.color: "green"
        border.width: 3
        z: 100
    }
    // 定时更新调试框位置（仅在调试模式下运行）
    Timer {
        id: uvDebugTimer
        interval: 500  // 500ms 更新一次
        running: showUvDebugRect
        repeat: true
        onTriggered: {
            uvDebugRect.x = ebutton.getIruvDstX()
            uvDebugRect.y = ebutton.getIruvDstY()
            uvDebugRect.width = ebutton.getIruvDstWidth()
            uvDebugRect.height = ebutton.getIruvDstHeight()
            uvDebugRect.visible = false
        }
    }

    Window {
            id: windowsss
            title: "警告"
            x:800
            y:400
            width: 300
            height: 200
            visible: false
            modality: Qt.ApplicationModal
            objectName: "windowsss"

            Text {
                text: "NO SD!"
                anchors.centerIn: parent
                font.pixelSize: 40
            }

            Button {
                text: "确定"
                x:100
                y:150
                onClicked: windowsss.close()
            }
        }

    // 背景矩形，填充一半
    Rectangle {
        id: batteryBackground
        width: 14
        height: 40
        color: "white" // 背景颜色
        x:1787
        y:15
    }

    Rectangle {
        id: batteryBackground2
        width: 14
        height: 40
        color: "white" // 背景颜色
        x:1803
        y:15
    }
    Rectangle {
        id: batteryBackground3
        width: 14
        height: 40
        color: "white" // 背景颜色
        x:1818
        y:15
    }
    Rectangle {
        id: batteryBackground4
        width: 14
        height: 40
        color: "white" // 背景颜色
        x:1833
        y:15
    }


        // 电池图形
        Item {
            width: 150
            height: 100
            anchors.horizontalCenter: parent.horizontalCenter

            // 使用自定义电池图像
            Image {
                id: batteryImage
                source: "/photo/dc.png" // 您的图片路径
                width: 80
                height: 40
                x:9005
                y:15
            }

            // 电量文本放置在电池内部
            Text {
                id: batteryLevelText
                // text: ebutton.readBatteryLevel() + "%"
                font.pointSize: 14
                color: "black"
                anchors.verticalCenter: batteryImage.verticalCenter
                anchors.left: batteryImage.left
                anchors.leftMargin: 14
            }
        }

    Text {
        id: testtr
        visible: true
        objectName:"gsname"
        anchors.top: parent.top
        anchors.left: parent.left
        color: "white"
        text: qsTr("紫红光电 UVIRSYS")
        //text: qsTr("MetaUVI")
        font.pixelSize: 65
        font.bold: true
        z: 2
    }
    Text {
        id: testtr_en
        visible: false
        y:85
        color: "white"
        text: qsTr("")
        font.pixelSize: 65
        font.bold: true
        z: 2
    }
    property string testtrchoose: ""
    property string shezhi_biaoti_1: "设置"
    property string yanseshezhi_biaoti_1: "颜色设置"

    // 红外模式画框功能相关
    property bool showIrOptions: false
    property bool showIrRectangle: false
    property int rectCounter: 0
    property double maxTemperature: 0.0

    ListModel {
        id: rectListModel
    }

    property bool showPseudoColorMenu: false  //伪彩
    property bool showDrawLineMenu: false  //画线菜单
    property bool showDrawPointMenu: false  //画点菜单
    property bool showIrLine: false  //显示画线
    property bool showIrPoint: false  //显示画点
    property int globalHotX: 0
    property int globalHotY: 0

    // 画点功能相关
    ListModel {
        id: pointListModel
    }
    property int pointCounter: 0

    // 画线功能相关
    property int lineX1: 560
    property int lineY1: 540
    property int lineX2: 1360
    property int lineY2: 540
    property int lineMidX: 960
    property int lineMidY: 540
    property bool isDraggingLine: false
    property bool isRotatingLine: false
    property int dragLineStartX: 0
    property int dragLineStartY: 0
    property int lineStartX1: 0
    property int lineStartY1: 0
    property int lineStartX2: 0
    property int lineStartY2: 0
    property int lineStartMidX: 0
    property int lineStartMidY: 0

    Text {
        id: testtrch
        objectName: "mingzi"
        visible: false
        x:10
        y:80
        color: "white"
        text: qsTr(testtrchoose)
        font.pixelSize: 55
        font.bold: true
        z: 2
    }


    property string gpsdata: "gpsdata："
    Text {
        id: gps
        objectName: "gps"
        visible: false
        anchors.top: testtr.top
        anchors.topMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 1150
        color: "white"
        text: qsTr(gpsdata)
        font.pixelSize: 50
        //font.bold: true
    }
    property bool gpsAvailable: true
    property bool wifiEnabled: false
    property string wifiStatusText: ""
    property bool wifiScanning: false
    property string connectedSSID: ""
    property string selectedSSID: ""
    property bool btEnabled: false
    property bool btScanning: false
    property string btStatusText: ""
    property string btConnectedDevice: ""
    property string selectedBtName: ""
    property string selectedBtAddr: ""
    property string connectedBtAddr: ""
    ListModel { id: btDeviceList }
    property bool fileServerRunning: false
    property string fileServerURL: ""
    property string gpsdata_te: "gpsdata："
    Text {
        id: gps_te
        visible: false
        anchors.top: testtr.top
        anchors.topMargin: 500
        anchors.left: parent.left
        anchors.leftMargin: 150
        color: "white"
        text: qsTr(gpsdata_te)
        font.pixelSize: 50
        //font.bold: true
    }

 property string shijian: "CST：2024/01/01 01:01:01"
    Text {
        id: testtr1
        objectName: "shijian1"
        visible: true
        x:1200
        y:1000
        color: "white"
        text: shijian
        font.pixelSize: 50
        //font.bold: true
        z: 2
    }
    property string dynamicTextws: "T/H：26℃/76%"
    Text {
        id: testtr2
        objectName: "wenshidu"
        x:650
        y:1000
        color: "white"
        text: dynamicTextws
        font.pixelSize: 50
        //font.bold: true
    }
    Text {
        id: testtr3
        objectName: "moshi"
        x:150
        y:1000
        color: "white"
        text: "模式：融合"
        font.pixelSize: 50
        //font.bold: true
    }

    property string dynamicText: "增益：50%"
    Text {
        id: testtr4
        objectName: "zengyi"
        x:150
        y:920
        color: "white"
        text: dynamicText
        font.pixelSize: 50
        visible: (ebutton.mssz !== 3 && ebutton.mssz !== 0)
        //font.bold: true
    }
    property string dynamicTextmm: "D：0mm"
    Text {
        id: testtr5
        objectName: "juli"
        x:650
        y:920
        color: "white"
        text: dynamicTextmm
        font.pixelSize: 50
        visible: (ebutton.mssz !== 3 && ebutton.mssz !== 0)
        //font.bold: true
    }

property string dynamicText1: "C：0/M"
    Text {
        id: testtr6
        objectName: "jishu"
        x:1200
        y:920
        color: "white"
        text: dynamicText1
        font.pixelSize: 50
        visible: (ebutton.mssz !== 3 && ebutton.mssz !== 0)
        //font.bold: true
    }

    Image {
        id: ruxian
        objectName: "ruxian"
        x:930
        y:30
        visible: false
        width: 50 // 右半部分宽度
        height: 50
        source: "/photo/录像.png" // 初始为空
        fillMode: Image.PreserveAspectFit // 保持图片纵横比
    }

    Image {
        id: baojing
        x:1750
        y:500
        visible: false
        width: 100 // 右半部分宽度
        height: 100
        source: "/photo/baojing.png" // 初始为空
        fillMode: Image.PreserveAspectFit // 保持图片纵横比
    }

    Rectangle {
        id: flashScreen
        objectName: "flashScreen"
        anchors.fill: parent
        color: "black"
        visible: false
        z: 9999
    }

    Timer {
        id: flashTimer
        objectName: "flashTimer"
        interval: 100
        running: false
        repeat: false
        onTriggered: {
            flashScreen.visible = false
            if (taskPanelList.photographedIndex >= 0) {
                contentModel.setProperty(taskPanelList.photographedIndex, "photographed", true)
                // 台账按下拍照按钮自动选中下一个
                    var nextIdx = taskPanelList.photographedIndex + 1
                    if (nextIdx < contentModel.count) {
                        taskPanelList.selectedIndex = nextIdx
                        taskPanelList.photographedIndex = nextIdx
                        testtrch.text = contentModel.get(nextIdx).lineText
                    }
            }
        }
    }


    Timer {
        id: ruxianti
        objectName: "ruxianti"
        interval: 800 // 每 0.5 秒触发一次
        running: false // 应用启动时自动开始运行
        repeat: true  // 使定时器重复触发

        onTriggered: {
            ruxian.visible = !ruxian.visible;
        }

    }

    Text{
        id:l_l
        objectName: "l_l"
        text: "00:00:00"
        visible: false
        x:1000
        y:25
        font.pixelSize: 55
        color: "white"
    }
    property int s_l:0
    property int f_l:0
    property int m_l:0
    Timer{
        objectName: "ji_s"
        interval: 1000
        running: false
        repeat: true
        onTriggered: {
            if(++m_l > 59){
                m_l = 0;
                f_l++;
            }
            if(f_l > 59)
                s_l++;

            if(s_l < 10)
                l_l.text = "0"+s_l.toString()
            else
                l_l.text = s_l.toString()
            if(f_l < 10)
                l_l.text = l_l.text+":"+"0"+f_l.toString()
            else
                l_l.text = l_l.text+":"+f_l.toString()
            if(m_l < 10)
                l_l.text = l_l.text+":"+"0"+m_l.toString()
            else
                l_l.text = l_l.text + ":" + m_l.toString()
        }
        onRunningChanged :{
            s_l = 0;
            f_l = 0;
            m_l = 0;
        }
    }


    Drawer {
        id: drawer
        objectName: "drawer"
        width: window.width
        height:0.15 * window.height
        edge: Qt.BottomEdge
        dragMargin: window.height/4
        modal: false
        z: 1

        onOpened: {
            if(testtr6.y == 920){
                testtr1.y = testtr1.y - 160
                testtr2.y = testtr2.y - 160
                testtr3.y = testtr3.y - 160
                testtr4.y = testtr4.y - 160
                testtr5.y = testtr5.y - 160
                testtr6.y = testtr6.y - 160
            }
        }

        onClosed: {
            if(testtr6.y == 760)
            {
                testtr1.y = testtr1.y + 160
                testtr2.y = testtr2.y + 160
                testtr3.y = testtr3.y + 160
                testtr4.y = testtr4.y + 160
                testtr5.y = testtr5.y + 160
                testtr6.y = testtr6.y + 160
            }
        }


        Timer {
            id: data
            interval: 500 // 每 0.5 秒触发一次
            running: true // 应用启动时自动开始运行
            repeat: true  // 使定时器重复触发

            onTriggered: {
                dynamicText1 = ebutton.getgz();
                dynamicTextws = ebutton.getws();
                dynamicTextmm = ebutton.getmm();
                gpsdata = ebutton.getgps();
                gpsdata_te = ebutton.getgps_te();
                dynamicText = ebutton.getzy();
                //ebutton.getrtc();

                //console.log("okokok--------");
                // batteryLevelText.text = ebutton.readBatteryLevel() + "%"

                if(ebutton.readBatteryLevel() >75){
                    batteryBackground.visible = true;
                    batteryBackground2.visible = true;
                    batteryBackground3.visible = true;
                    batteryBackground4.visible = true;
                }
                else if(ebutton.readBatteryLevel() >50){
                    batteryBackground.visible = true;
                    batteryBackground2.visible = true;
                    batteryBackground3.visible = true;
                    batteryBackground4.visible = false;
                }
                else if(ebutton.readBatteryLevel() >25){
                    batteryBackground.visible = true;
                    batteryBackground2.visible = true;
                    batteryBackground3.visible = false;
                    batteryBackground4.visible = false;
                }
                else if(ebutton.readBatteryLevel() >5)
                {
                    batteryBackground.visible = true;
                    batteryBackground2.visible = false;
                    batteryBackground3.visible = false;
                    batteryBackground4.visible = false;
                }
                else if(ebutton.readBatteryLevel() >0){
                    batteryBackground.visible = false;
                    batteryBackground2.visible = false;
                    batteryBackground3.visible = false;
                    batteryBackground4.visible = false;
                }
                // m_first_photons = ebutton.get1();
                // m_second_photons= ebutton.get2();
                // m_third_photons= ebutton.get3();
                // m_fourth_photons= ebutton.get4();

            }
        }
        Timer {
            id:q_time
            interval: 500
            running: false
            repeat: true
            onTriggered: {
                if(ebutton.getguagzishu()>quezhi.currentIndex*1000){
                    baojing.visible =true;
                    ebutton.playAlarm();
                }else{
                    baojing.visible = false;
                    ebutton.stopAlarm();
                }
            }
        }

        Timer {
            id: times
            interval: 1000
            running: true
            repeat: true
            onTriggered: {
                shijian = ebutton.gettime();
            }

        }

        Timer{
            interval: 1
            running: true
            repeat: false
            onTriggered: {
                fileValue = ebutton.readDigitFromFile("/opt/l_mv");
                if(fileValue == 0){
                    cn_l.running = true;
                }else if(fileValue == 1)
                {
                    en_l.running = true;
                }
                else if(fileValue == 4){
                    rn_l.running = true;
                }
                else if(fileValue == 5){
                    ydl_l.running = true;
                }
                else
                {
                    eyun_l.running = true;
                }


                // 恢复持久化设置
                var jfVal = ebutton.readDigitFromFile("/opt/jf_mv");
                listView_jifen_nei.currentIndex = jfVal;

                var jishuVal = ebutton.readDigitFromFile("/opt/jishu_mv");
                listView_jishu_nei.currentIndex = jishuVal;

                var cejuVal = ebutton.readDigitFromFile("/opt/ceju_mv");
                listView_ceju_nei.currentIndex = cejuVal;

                var quezhiVal = ebutton.readDigitFromFile("/opt/quezhi_mv");
                var quezhiIdx = quezhiVal / 1000;
                listView_quezhi_nei.currentIndex = quezhiIdx;
                quezhi.currentIndex = quezhiIdx;

                var quezhiSw = ebutton.readDigitFromFile("/opt/quezhi_sw_mv");
                if (quezhiSw === 1) {
                    quezhi_switch_btn.text = "ON";
                    q_time.start();
                } else {
                    quezhi_switch_btn.text = "OFF";
                    q_time.stop();
                }

                var gpsVal = ebutton.readDigitFromFile("/opt/gps_mv");
                listView_gps.currentIndex = gpsVal;
                if (gpsVal === 0) {
                    gps.visible = true;
                } else {
                    gps.visible = false;
                }
            }

        }

        Timer{
            id:en_l
            interval: 1
            running: false
            repeat: false
            onTriggered: {
            try {
            console.log("[DEBUG] en_l onTriggered START");
            ebutton.yuyan = 1;
            ebutton.setyuyan(1);
            listView.model.setProperty(0, "name", "Red");
            listView.model.setProperty(1, "name", "White");
            listView.model.setProperty(2, "name", "Blue");
            listView.model.setProperty(3, "name", "Yellow");
            listView.model.setProperty(4, "name", "Cyan");
            listView.model.setProperty(5, "name", "Magenta");
            listView.model.setProperty(6, "name", "Pink");
            listView.model.setProperty(7, "name", "Purple");
            listView.model.setProperty(8, "name", "Gray");
            listView.model.setProperty(9, "name", "Dark Green");
            listView.model.setProperty(10, "name", "Light Blue");
            listView.model.setProperty(11, "name", "Green");
            listView1.model.setProperty(0, "name", "Points");
            listView1.model.setProperty(1, "name", "Counting Unit");
            listView1.model.setProperty(2, "name", "Threshold");
            listView1.model.setProperty(3, "name", "Ledger Function");
            listView1.model.setProperty(4, "name", "Distance Switch");
            listView1.model.setProperty(5, "name", "Wi-Fi/Bluetooth");
            listView1.model.setProperty(6, "name", "Language");
            listView1.model.setProperty(7, "name", "Time Settings");
            listView1.model.setProperty(8, "name", "Positioning Switch");
            factorCorrectionTitle.text = "Factor Correction";
            shezhi_biaoti_1 = "set up";
            yanseshezhi_biaoti_1 = "color set";
            testtr.text = "UVIRSYS";
            //quezhi_1.text = "threshold";
            thresholdInputDialog.title = "Set threshold";
            thresholdInputDialog_1.text = "Input threshold(0-10000)";
            queding_2.text = "sure";
            quxiao_2.text = "cancel";
            taizhang.title = "Import ledger";
            taizhang.taskTabText = "Tasks";
            taizhang.ledgerTabText = "Ledger List";
            taizhang.startTaskText = "Start Task";
            taizhang.stopTaskText = "Stop Task";
            taizhang.currentTaskText = "Current Task";
            taizhang.noFileText = "Not Selected";
            taizhang.titleText = "Ledger";
            datetimeInputDialog.title = "Enter date and time";
            time_ok.text = "sure";
            time_off.text = "cancel";
            dialog.title = "Folder";
            fullScreenDialog.title = "Full-screen image";

            pointTitle.text = "Points Settings";
            countTitle.text = "Counting Unit";
            thresholdTitle.text = "Threshold"
            distanceTitle.text = "Distance Switch"
            wifiBtTitle.text = "Wi-Fi/Bluetooth"
            languageTitle.text = "Language"
            timeTitle.text = "Time Settings"
            positionTitle.text = "Positioning Switch"

            //ebutton.switch_l(0);
            testtr.font.pixelSize = 60;
            ebutton.setyuyan(1);
            ebutton.setzywen(1);
            dynamicText = ebutton.getzy();
            ebutton.switch_l(0);
            updateModeText()
            } catch(e) {
                console.log("[ERROR] en_l crash: " + e)
            }
            }
        }


        Timer{
            id:cn_l
            interval: 1
            running: false
            repeat: false
            onTriggered: {

                try {
                    ebutton.yuyan = 0;
                    ebutton.setyuyan(0);
                    listView.model.setProperty(0, "name", "红");
                    listView.model.setProperty(1, "name", "白");
                    listView.model.setProperty(2, "name", "蓝");
                    listView.model.setProperty(3, "name", "黄");
                    listView.model.setProperty(4, "name", "青");
                    listView.model.setProperty(5, "name", "品红");
                    listView.model.setProperty(6, "name", "玫红");
                    listView.model.setProperty(7, "name", "紫");
                    listView.model.setProperty(8, "name", "灰");
                    listView.model.setProperty(9, "name", "深绿");
                    listView.model.setProperty(10, "name", "淡蓝");
                    listView.model.setProperty(11, "name", "绿");
                    listView1.model.setProperty(0, "name", "积分");
                    listView1.model.setProperty(1, "name", "计数单位");
                    listView1.model.setProperty(2, "name", "阈值");
                    listView1.model.setProperty(3, "name", "台账功能");
                    listView1.model.setProperty(4, "name", "测距开关");
                    listView1.model.setProperty(5, "name", "wifi/蓝牙");
                    listView1.model.setProperty(6, "name", "语言");
                    listView1.model.setProperty(7, "name", "时间设置");
                    listView1.model.setProperty(8, "name", "定位开关");
                    factorCorrectionTitle.text = "因子矫正";
                    shezhi_biaoti_1 = "设置";
                    yanseshezhi_biaoti_1 = "颜色设置";
                    testtr.text = "紫红光电 UVIRSYS";
                    thresholdInputDialog.title = "设置阈值";
                    thresholdInputDialog_1.text = "输入阈值（0-10000）";
                    queding_2.text = "确定";
                    quxiao_2.text = "取消";
                    taizhang.title = "导入台账";
                    taizhang.taskTabText = "任务";
                    taizhang.ledgerTabText = "台账列表";
                    taizhang.startTaskText = "启动任务";
                    taizhang.stopTaskText = "停止任务";
                    taizhang.currentTaskText = "当前任务";
                    taizhang.noFileText = "未选择";
                    taizhang.titleText = "台账管理";
                    datetimeInputDialog.title = "输入日期和时间";
                    time_ok.text = "确定";
                    time_off.text = "取消";
                    dialog.title = "文件夹";
                    fullScreenDialog.title = "全屏图像";

                    pointTitle.text = "积分设置";
                    countTitle.text = "计数单位";
                    thresholdTitle.text = "阈值";
                    distanceTitle.text = "测距开关";
                    wifiBtTitle.text = "Wi-Fi/蓝牙";
                    languageTitle.text = "语言";
                    timeTitle.text = "时间设置";
                    positionTitle.text = "定位开关";
                    timeConfirm.text = "确定";
                    timeCancel.text = "取消";

                    testtr.font.pixelSize = 65;
                    ebutton.setzywen(0);
                    ebutton.setyuyan(0);
                    dynamicText = ebutton.getzy();
                    ebutton.switch_l(1);
                    updateModeText()
                } catch(e) {
                    console.log("[ERROR] cn_l crash: " + e)
                }
                }
        }

        Timer{
            id:rn_l
            interval: 1
            running: false
            repeat: false
            onTriggered: {

                    try {
                    ebutton.yuyan = 4;
                    ebutton.setyuyan(4);
                    listView.model.setProperty(0, "name", "レッド");
                    listView.model.setProperty(1, "name", "ホワイト");
                    listView.model.setProperty(2, "name", "ブルー");
                    listView.model.setProperty(3, "name", "イエロー");
                    listView.model.setProperty(4, "name", "スカイブルー");
                    listView.model.setProperty(5, "name", "マゼンタ");
                    listView.model.setProperty(6, "name", "ローズレッド");
                    listView.model.setProperty(7, "name", "パープル");
                    listView.model.setProperty(8, "name", "灰（はい）");
                    listView.model.setProperty(9, "name", "深緑");
                    listView.model.setProperty(10, "name", "ディープブルー");
                    listView.model.setProperty(11, "name", "グリーン");
                    listView1.model.setProperty(0, "name", "積分");
                    listView1.model.setProperty(1, "name", "カウント単位");
                    listView1.model.setProperty(2, "name", "しきい値");
                    listView1.model.setProperty(3, "name", "台帳機能");
                    listView1.model.setProperty(4, "name", "レンジスイッチ");
                    listView1.model.setProperty(5, "name", "wifi/Blue");
                    listView1.model.setProperty(6, "name", "言語");
                    listView1.model.setProperty(7, "name", "時刻設定");
                    listView1.model.setProperty(8, "name", "位置スイッチ");
                    factorCorrectionTitle.text = "係数補正";
                    shezhi_biaoti_1 = "設定";
                    yanseshezhi_biaoti_1 = "色設定";
                    testtr.text = "UVIRSYS";
                    //testtr.text = "武高电测";
                    //quezhi_1.text = "阈值";
                    thresholdInputDialog.title = "しきい値の設定";
                    thresholdInputDialog_1.text = "しきい値の入力（0～10000）";
                    queding_2.text = "確定";
                    quxiao_2.text = "キャンセル";
                    taizhang.title = "導入台帳";
                    taizhang.taskTabText = "タスク";
                    taizhang.ledgerTabText = "台帳リスト";
                    taizhang.startTaskText = "開始";
                    taizhang.stopTaskText = "停止";
                    taizhang.currentTaskText = "現在のタスク";
                    taizhang.noFileText = "未選択";
                    taizhang.titleText = "台帳管理";
                    datetimeInputDialog.title = "日付と時刻の入力";
                    time_ok.text = "確定";
                    time_off.text = "キャンセル";
                    dialog.title = "フォルダ";
                    fullScreenDialog.title = "全画面画像";

                    pointTitle.text = "ポイント設定";
                    countTitle.text = "カウント単位";
                    thresholdTitle.text = "しきい値";
                    distanceTitle.text = "距離測定スイッチ";
                    wifiBtTitle.text = "Wi-Fi/Bluetooth";
                    languageTitle.text = "言語";
                    timeTitle.text = "時刻設定";
                    positionTitle.text = "位置情報スイッチ";
                    timeConfirm.text = "確定";
                    timeCancel.text = "キャンセル";

                    testtr.font.pixelSize = 65;
                    ebutton.setzywen(1);
                    ebutton.setyuyan(4);
                    dynamicText = ebutton.getzy();
                    ebutton.switch_l(0);
                    updateModeText()
                } catch(e) {
                    console.log("[ERROR] rn_l crash: " + e)
                }
                }
        }
        Timer{
            id:eyun_l
            interval: 1
            running: false
            repeat: false
            onTriggered: {
                try {
                console.log("sign0")
                ebutton.yuyan = 2;
                ebutton.setyuyan(2);
                listView.model.setProperty(0, "name", "Красный");
                listView.model.setProperty(1, "name", "Белый");
                listView.model.setProperty(2, "name", "Синий");
                listView.model.setProperty(3, "name", "Желтый");
                listView.model.setProperty(4, "name", "Циан");
                listView.model.setProperty(5, "name", "Магента");
                listView.model.setProperty(6, "name", "Розовый");
                listView.model.setProperty(7, "name", "Фиолетовый");
                listView.model.setProperty(8, "name", "Серый");
                listView.model.setProperty(9, "name", "Темно-зеленый");
                listView.model.setProperty(10, "name", "Светло-голубой");
                listView.model.setProperty(11, "name", "Зеленый");
                listView1.model.setProperty(0, "name", "Очки");
                listView1.model.setProperty(1, "name", "Единица измерения");
                listView1.model.setProperty(2, "name", "Порог");
                listView1.model.setProperty(3, "name", "Функция бухгалтерского учета");
                listView1.model.setProperty(4, "name", "Переключатель расстояния");
                listView1.model.setProperty(5, "name", "Wi-Fi/Bluetooth");
                listView1.model.setProperty(6, "name", "Язык");
                listView1.model.setProperty(7, "name", "Настройки времени");
                listView1.model.setProperty(8, "name", "позиционный переключатель");
                factorCorrectionTitle.text = "Коррекция факторов";
                shezhi_biaoti_1 = "настройка";
                yanseshezhi_biaoti_1 = "установка цвета";
                testtr.text = "UVIRSYS";
                // quezhi_1.text = "порог";
                thresholdInputDialog.title = "Установить порог";
                thresholdInputDialog_1.text = "Введите порог (0-10000)";
                queding_2.text = "подтвердить";
                quxiao_2.text = "отменить";
                taizhang.title = "Импорт учёта";
                taizhang.taskTabText = "Задачи";
                taizhang.ledgerTabText = "Реестр";
                taizhang.startTaskText = "Запуск";
                taizhang.stopTaskText = "Стоп";
                taizhang.currentTaskText = "Тек. задача";
                taizhang.noFileText = "Не выбрано";
                taizhang.titleText = "Учёт";
                datetimeInputDialog.title = "Введите дату и время";
                time_ok.text = "подтвердить";
                time_off.text = "отменить";
                dialog.title = "папка";
                fullScreenDialog.title = "Полноэкранное изображение";

                pointTitle.text = "Настройки баллов";
                countTitle.text = "Единица счёта";
                //thresholdTitle.text = "Порог";
                distanceTitle.text = "Переключатель дальности";
                wifiBtTitle.text = "Wi-Fi/Bluetooth";
                languageTitle.text = "Язык";
                timeTitle.text = "Настройки времени";
                positionTitle.text = "Переключатель геолокации";
                timeConfirm.text = "Подтвердить";
                timeCancel.text = "Отмена";
                console.log("sign1")
                testtr.font.pixelSize = 60;
                ebutton.setyuyan(2);
                console.log("sign2")
                ebutton.setzywen(1);
                console.log("sign3")
                dynamicText = ebutton.getzy();
                console.log("sign4")
                ebutton.switch_l(0);
                console.log("sign5")
                updateModeText()
                } catch(e) {
                    console.log("[ERROR] eyun_l crash: " + e)
                }
                }
        }



        Timer{
            id:ydl_l
            interval: 1
            running: false
            repeat: false
            onTriggered: {

                try {
                ebutton.yuyan = 5;
                ebutton.setyuyan(5);
                listView.model.setProperty(0, "name", "Rosso");
                listView.model.setProperty(1, "name", "Bianco");
                listView.model.setProperty(2, "name", "Blu");
                listView.model.setProperty(3, "name", "Giallo");
                listView.model.setProperty(4, "name", "Ciano");
                listView.model.setProperty(5, "name", "Magenta");
                listView.model.setProperty(6, "name", "Rosso porpora");
                listView.model.setProperty(7, "name", "Viola");
                listView.model.setProperty(8, "name", "Grigio");
                listView.model.setProperty(9, "name", "Verde scuro");
                listView.model.setProperty(10, "name", "Azzurro chiaro");
                listView.model.setProperty(11, "name", "Verde");

                listView1.model.setProperty(0, "name", "Punti");
                listView1.model.setProperty(1, "name", "Unità di conteggio");
                listView1.model.setProperty(2, "name", "Soglia");
                listView1.model.setProperty(3, "name", "Registro");
                listView1.model.setProperty(4, "name", "Interruttore distanza");
                listView1.model.setProperty(5, "name", "WiFi/Bluetooth");
                listView1.model.setProperty(6, "name", "Lingua");
                listView1.model.setProperty(7, "name", "Imposta data/ora");
                listView1.model.setProperty(8, "name", "interruttore di posizionamento");
                factorCorrectionTitle.text = "Correzione fattori";
                shezhi_biaoti_1 = "Impostazioni";
                yanseshezhi_biaoti_1 = "colore";
                testtr.text = "UVIRSYS";
                thresholdInputDialog.title = "Imposta soglia";
                thresholdInputDialog_1.text = "Inserisci soglia (0-10000)";
                queding_2.text = "Conferma";
                quxiao_2.text = "Annulla";
                taizhang.title = "Importa registro";
                taizhang.taskTabText = "Compiti";
                taizhang.ledgerTabText = "Elenco";
                taizhang.startTaskText = "Avvia";
                taizhang.stopTaskText = "Ferma";
                taizhang.currentTaskText = "Compito";
                taizhang.noFileText = "Nessuno";
                taizhang.titleText = "Registro";
                datetimeInputDialog.title = "Imposta data e ora";
                time_ok.text = "Conferma";
                time_off.text = "Annulla";
                dialog.title = "Cartella";
                fullScreenDialog.title = "Immagine a schermo intero";

                pointTitle.text = "Impostazioni punti";
                countTitle.text = "Unità di conteggio";
                thresholdTitle.text = "Soglia";
                distanceTitle.text = "Interruttore distanza";
                wifiBtTitle.text = "Wi-Fi/Bluetooth";
                languageTitle.text = "Lingua";
                timeTitle.text = "Impostazioni orario";
                positionTitle.text = "Interruttore posizione";
                timeConfirm.text = "Conferma";
                timeCancel.text = "Annulla";

                testtr.font.pixelSize = 65;
                ebutton.setzywen(1);
                ebutton.setyuyan(5);
                dynamicText = ebutton.getzy();
                ebutton.switch_l(0);

                updateModeText()
            } catch(e) {
                console.log("[ERROR] ydl_l crash: " + e)
            }
            }
        }


        Button {
            id: button1
            objectName: "button1"
            width: 150
            height: 150
            onClicked: {
                ebutton.myFunction();
                border1.visible = true;
                border2.visible = false;
                border3.visible = false;
                border4.visible = false;
                border5.visible = false;
                border6.visible = false;
                border7.visible = false;
                border8.visible = false;
                border9.visible = false;

            }

            font {
                pixelSize: 24
            }

            Image {
                source: "/photo/变焦2.png"
                fillMode: Image.Stretch
                width: button1.width
                height: button1.height
                clip: true
            }
            Rectangle {
                id: border1
                objectName: "border1"
                anchors.fill: parent
                border.width: 2
                border.color: "green"
                color: "transparent"
                visible: false
            }
            anchors {
                top: parent.top
                left: parent.left
                leftMargin: 50
            }
            background: Rectangle {
                color: "transparent"
                width: button1.width
                height: button1.height
            }
        }

        Button {
            id: button2
            objectName: "button2"
            //text: "变焦-"
            width: 150
            height: 150
            onClicked: {
                ebutton.anotherFunction();
                border1.visible = false;
                border2.visible = true;
                border3.visible = false;
                border4.visible = false;
                border5.visible = false;
                border6.visible = false;
                border7.visible = false;
                border8.visible = false;
                border9.visible = false;

            }
            font {
                pixelSize: 24
            }

            Rectangle {
                id: border2
                objectName: "border2"
                anchors.fill: parent
                border.width: 2
                border.color: "green"
                color: "transparent"
                visible: false
            }
            anchors {
                top: button1.top
                left: button1.right
                leftMargin: 50
            }


            background: Rectangle {
                color: "transparent"
                width: button2.width
                height: button2.height
            }

            Image {
                //source: "/photo/变焦3.jpg"
                source: "/photo/--.png"
                fillMode: Image.Stretch
                width: button2.width
                height: button2.height
            }

        }

        Button {
            id: button3
            objectName: "button3"
            //text: "增益+"
            width: 150
            height: 150
            onClicked: {
                ebutton.myFunction1();
                dynamicText = ebutton.getzy();
                border1.visible = false;
                border2.visible = false;
                border3.visible = true;
                border4.visible = false;
                border5.visible = false;
                border6.visible = false;
                border7.visible = false;
                border8.visible = false;
                border9.visible = false;
            }

            onPressed: {
                myTimer1.start();
            }

            onReleased: {
                myTimer1.stop();
            }

            font {
                pixelSize: 24
            }
            Rectangle {
                id: border3
                objectName: "border3"
                anchors.fill: parent
                border.width: 2
                border.color: "green"
                color: "transparent"
                visible: false
            }
            anchors {
                top: button2.top
                left: button2.right
                leftMargin: 50
            }
            background: Rectangle {
                color: "transparent"
                width: button3.width
                height: button3.height
            }
            Image {
                source: "/photo/z++.png"
                fillMode: Image.Stretch
                width: button3.width
                height: button3.height
            }

            Timer {
                id: myTimer1
                interval: 200 // 调用函数的频率，以毫秒为单位
                running: false
                repeat: true
                onTriggered: {
                    ebutton.myFunction1();
                    dynamicText = ebutton.getzy();
                }
            }

        }

        Button {
            id: button4
            objectName: "button4"
            //text: "增益-"
            width: 150
            height: 150
            onClicked: {
                ebutton.anotherFunction1();
                dynamicText = ebutton.getzy();
                border1.visible = false;
                border2.visible = false;
                border3.visible = false;
                border4.visible = true;
                border5.visible = false;
                border6.visible = false;
                border7.visible = false;
                border8.visible = false;
                border9.visible = false;

            }
            font {
                pixelSize: 24
            }
            onPressed: {
                myTimer2.start();
            }

            onReleased: {
                myTimer2.stop();
            }
            Rectangle {
                id: border4

                objectName: "border4"
                anchors.fill: parent
                border.width: 2
                border.color: "green"
                color: "transparent"
                visible: false
            }
            anchors {
                top: button3.top
                left: button3.right
                leftMargin: 50
            }
            background: Rectangle {
                color: "transparent"
                width: button4.width
                height: button4.height
            }
            Image {
                source: "/photo/z--.png"
                fillMode: Image.Stretch
                width: button4.width
                height: button4.height
            }
            Timer {
                id: myTimer2
                interval: 200 // 调用函数的频率，以毫秒为单位
                running: false
                repeat: true
                onTriggered: {
                    ebutton.anotherFunction1();
                    dynamicText = ebutton.getzy();
                }
            }

        }

        Button {
            id: button5
            objectName: "button5"
            //text: "计数"
            width: 150
            height: 150
            onClicked: {
                var i = ebutton.setjs();
                if(i===2){
                    // m_first_photon.visible = true;
                    // m_second_photon.visible= true;
                    // m_third_photon.visible= true;
                    // m_fourth_photon.visible= true;
                }else{
                    // m_first_photon.visible = false;  m_second_photon.visible= false;  m_third_photon.visible= false;  m_fourth_photon.visible= false;
                }
                border1.visible = false;
                border2.visible = false;
                border3.visible = false;
                border4.visible = false;
                border5.visible = true;
                border6.visible = false;
                border7.visible = false;
                border8.visible = false;
                border9.visible = false;
            }
            font {
                pixelSize: 24
            }
            Rectangle {
                id: border5
                objectName: "border5"
                anchors.fill: parent
                border.width: 2
                border.color: "green"
                color: "transparent"
                visible: false
            }
            anchors {
                top: button4.top
                left: button4.right
                leftMargin: 50
            }
            background: Rectangle {
                color: "transparent"
                width: button5.width
                height: button5.height
            }
            Image {
                source: "/photo/js.png"
                fillMode: Image.Stretch
                width: button5.width
                height: button5.height
            }

        }



        Button {
            id: button6
            objectName: "button6"
            width: 150
            height: 150
            onClicked: {
                console.log("yuyan:" + ebutton.getyuyan())
                //ebutton.bgra8888();
                //displayImage.visible = true;
                // ebutton.overluzhi();
                border1.visible = false;
                border2.visible = false;
                border3.visible = false;
                border4.visible = false;
                border5.visible = false;
                border6.visible = true;
                border7.visible = false;
                border8.visible = false;
                border9.visible = false;

                showIrOptions = false;

                ebutton.moshiqiehuan();
                updateModeText()

            }
            font {
                pixelSize: 24
            }

            Rectangle {
                id: border6
                                objectName: "border6"
                anchors.fill: parent
                border.width: 2
                border.color: "green"
                color: "transparent"
                visible: false
            }
            anchors {
                top: button5.top
                left: button5.right
                leftMargin: 50
            }
            background: Rectangle {
                color: "transparent"
                width: button6.width
                height: button6.height
            }
            Image {
                source: "/photo/msqh.png"
                fillMode: Image.Stretch
                width: button5.width
                height: button5.height
            }

        }
        Button {
            id: button7
            objectName: "button7"
            width: 150
            height: 150
            onClicked: {
                // ebutton.startluzhi();
                drawer.close();
                border1.visible = false;
                border2.visible = false;
                border3.visible = false;
                border4.visible = false;
                border5.visible = false;
                border6.visible = false;
                border7.visible = true;
                border8.visible = false;
                border9.visible = false;

                // 关闭设置界面，避免透明窗口遮挡
                shezhi_jiemian.visible = false;

                // 直接打开文件管理对话框
                dialog.open();

            }







            font {
                pixelSize: 24
            }

            Rectangle {
                id: border7
                objectName: "border7"
                anchors.fill: parent
                border.width: 2
                border.color: "green"
                color: "transparent"
                visible: false
            }
            anchors {
                top: button6.top
                left: button6.right
                leftMargin: 50
            }
            background: Rectangle {
                color: "transparent"
                width: button7.width
                height: button7.height
            }
            Image {
                source: "/photo/bc.png"
                fillMode: Image.Stretch
                width: button5.width
                height: button5.height
            }

        }

        Button {
            id: button8
            objectName: "button8"
            width: 150
            height: 150
            onClicked: {
                drawer.close();
                //yanse.visible =true;
                yanse_jiemian.visible = true;
                border1.visible = false;
                border2.visible = false;
                border3.visible = false;
                border4.visible = false;
                border5.visible = false;
                border6.visible = false;
                border7.visible = false;
                border8.visible = true;
                border9.visible = false;
            }
            font {
                pixelSize: 24
            }
            Rectangle {
                id: border8
                objectName: "border8"
                anchors.fill: parent
                border.width: 2
                border.color: "green"
                color: "transparent"
                visible: false
            }
            anchors {
                top: button7.top
                left: button7.right
                leftMargin: 50
            }
            background: Rectangle {
                color: "transparent"
                width: button8.width
                height: button8.height
            }
            Image {
                source: "/photo/ys.png"
                fillMode: Image.Stretch
                width: button5.width
                height: button5.height
            }

        }


        Button {
            id: button9
            objectName: "button9"
            //text: "计数"
            width: 150
            height: 150
            onClicked: {
                // settingsDialog.open();
                drawer.close();
                //settingsDialog2222.visible = true;
                shezhi_jiemian.visible = true;
                border1.visible = false;
                border2.visible = false;
                border3.visible = false;
                border4.visible = false;
                border5.visible = false;
                border6.visible = false;
                border7.visible = false;
                border8.visible = false;
                border9.visible = true;
            }
            font {
                pixelSize: 24
            }
            Rectangle {
                id: border9
                objectName: "border9"
                anchors.fill: parent
                border.width: 2
                border.color: "green"
                color: "transparent"
                visible: false
            }
            anchors {
                top: button8.top
                left: button8.right
                leftMargin: 50
            }
            background: Rectangle {
                color: "transparent"
                width: button9.width
                height: button9.height
            }
            Image {
                source: "/photo/sz.png"
                fillMode: Image.Stretch
                width: button5.width
                height: button5.height
            }
        }

        Overlay.modal: Rectangle {
            color: "#010203"
        }

        background: Rectangle {
            color: "#010203"

            // Rectangle {
            //     width: parent.width
            //     height: 5
            //     color: "#ffffff"
            // }

        }

    }
    property string list_filename: ""
    property int fileValue: 0
    Dialog {
        id: dialog
        width: 1920
        height: 1080
        objectName: "dialog"
        font {
            pixelSize: 40
        }
        title: "文件夹"
        standardButtons: Dialog.Ok | Dialog.Cancel

        // ========== 属性定义（必须在最前面） ==========
        property string list_filename: ""
        property bool isVideoPreview: false
        property string currentVideoPath: ""

        FolderListModel {
            id: folderListModel
            folder: "file:///mnt/sdcard/Standard Storage"
            showDirs: true
            showDotAndDotDot: false
        }

        // 对话框打开时根据语言设置更新文本
        onOpened: {
            var lang = ebutton.getyuyan();
            if (lang === 1) { // 英文
                title = "Folder";
                imgList.text = "Image List";
                videoList.text = "Video List";
            }
            else if(lang === 0){
                title = "文件夹";
                imgList.text = "图片列表";
                videoList.text = "视频列表";
            }
            else if(lang === 4){
                title = "フォルダー";
                imgList.text = "画像リスト";
                videoList.text = "ビデオリスト";
            }
            else if(lang === 5){
                title = "Cartella";
                imgList.text = "Elenco delle immagini";
                videoList.text = "Elenco video";
            }
            else{
                title = "папка";
                imgList.text = "Список изображений";
                videoList.text = "Список видео";
            }
        }

        // 主布局：左右分栏
        Row {
            anchors.fill: parent
            spacing: 10

            // ========== 左侧：文件列表 ==========
            Column {
                width: parent.width * 0.45
                height: parent.height
                spacing: 10
                anchors.margins: 10

                // 上方：图片列表容器
                Rectangle {
                    width: parent.width
                    height: parent.height * 0.48
                    color: "#FAFAFA"
                    border.color: "#4CAF50"
                    border.width: 2
                    radius: 8

                    Column {
                        anchors.fill: parent
                        anchors.margins: 10

                        Text {
                            id: imgList
                            text: "图片列表"
                            font.pixelSize: 32
                            font.bold: true
                            color: "#4CAF50"
                        }

                        ListView {
                            id: imageListView
                            width: parent.width
                            height: parent.height - 40
                            model: folderListModel
                            clip: true

                            delegate: Rectangle {
                                width: imageListView.width
                                height: {
                                    var name = fileName.toLowerCase()
                                    var isimg = name.endsWith(".jpg") || name.endsWith(".png") || name.endsWith(".jpeg") || name.endsWith(".bmp")
                                    return isimg ? 60 : 0  // 不是视频则高度为0
                                }
                                visible: {
                                    var name = fileName.toLowerCase()
                                    return name.endsWith(".jpg") || name.endsWith(".png") || name.endsWith(".jpeg") || name.endsWith(".bmp")
                                }
                                color: mouseArea_img.pressed ? "#E0E0E0" : (dialog.list_filename === fileName && !dialog.isVideoPreview ? "#C8E6C9" : "transparent")

                                Behavior on color {
                                    ColorAnimation { duration: 120 }
                                }
                                MouseArea {
                                    id: mouseArea_img
                                    anchors.fill: parent
                                    onClicked: {
                                        console.log("点击图片: " + fileName)
                                        pt_filename.text = "" + fileName
                                        previewImage.source = folderListModel.folder + "/" + fileName.replace(/#/g, "%23")
                                        dialog.list_filename = fileName
                                        dialog.isVideoPreview = false
                                        dialog.currentVideoPath = ""
                                    }
                                }

                                Text {
                                    text: fileName
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    font.pixelSize: 28
                                    color: "#333"
                                }
                            }
                        }
                    }
                }

                // 下方：视频列表容器
                Rectangle {
                    width: parent.width
                    height: parent.height * 0.48
                    color: "#FAFAFA"
                    border.color: "#2196F3"
                    border.width: 2
                    radius: 8

                    Column {
                        anchors.fill: parent
                        anchors.margins: 10

                        Text {
                            id:videoList
                            text: "视频列表"
                            font.pixelSize: 32
                            font.bold: true
                            color: "#2196F3"
                        }

                        ListView {
                            id: videoListView
                            width: parent.width
                            height: parent.height - 40
                            model: folderListModel
                            clip: true

                            delegate: Rectangle {
                                width: videoListView.width
                                height: {
                                    var name = fileName.toLowerCase()
                                    var isVideo = name.endsWith(".mp4") || name.endsWith(".avi") ||
                                                 name.endsWith(".mov") || name.endsWith(".mkv")
                                    return isVideo ? 60 : 0  // 不是视频则高度为0
                                }
                                visible: {
                                    var name = fileName.toLowerCase()
                                    return name.endsWith(".mp4") || name.endsWith(".avi") || name.endsWith(".mov") || name.endsWith(".mkv")
                                }
                                color: mouseArea_video.pressed ? "#E0E0E0" : (dialog.list_filename === fileName && dialog.isVideoPreview ? "#BBDEFB" : "transparent")

                                Behavior on color {
                                    ColorAnimation { duration: 120 }
                                }
                                MouseArea {
                                    id: mouseArea_video
                                    anchors.fill: parent
                                    onClicked: {
                                        console.log("点击视频: " + fileName)
                                        pt_filename.text = fileName
                                        dialog.list_filename = fileName
                                        dialog.isVideoPreview = true
                                        dialog.currentVideoPath = "/mnt/sdcard/Standard Storage/" + fileName

                                        // 先显示默认播放图标
                                        previewImage.source = "file:///opt/bofang.jpg"

                                        // 尝试获取视频缩略图
                                        var thumbnailPath = ebutton.getVideoThumbnail(dialog.currentVideoPath)
                                        console.log("缩略图路径: " + thumbnailPath)
                                        if (thumbnailPath !== "file:///opt/bofang.jpg") {
                                            previewImage.source = thumbnailPath
                                        }
                                    }
                                }

                                Text {
                                    text: fileName
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    font.pixelSize: 28
                                    color: "#333"
                                }
                            }
                        }
                    }
                }
            }

            // 分隔线
            Rectangle {
                width: 2
                height: parent.height * 0.9
                color: "#DDD"
                anchors.verticalCenter: parent.verticalCenter
            }

            // ========== 右侧：预览区域 ==========
            Rectangle {
                width: parent.width * 0.5
                height: parent.height
                color: "#FAFAFA"

                Column {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 20

                    Rectangle {
                        width: parent.width
                        height: 60
                        color: "#F5F5F5"
                        border.color: "#E0E0E0"
                        radius: 8

                        Text {
                            id: pt_filename
                            anchors.centerIn: parent
                            text: "请选择文件预览"
                            font.pixelSize: 32
                            color: "#999"
                        }
                    }

                    Image {
                        id: previewImage
                        width: parent.width * 0.95
                        height: parent.height - 100
                        source: ""
                        fillMode: Image.PreserveAspectFit
                        anchors.horizontalCenter: parent.horizontalCenter

                        Rectangle {
                            anchors.fill: parent
                            color: "transparent"
                            border.color: "#E0E0E0"
                            border.width: 2
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                console.log("点击预览，isVideoPreview: " + dialog.isVideoPreview)
                                if (dialog.isVideoPreview) {
                                    // 播放视频
                                    console.log("播放视频: " + dialog.currentVideoPath)
                                    ebutton.bofangshipin(dialog.currentVideoPath)
                                } else if (previewImage.source !== "") {
                                    // 全屏预览图片
                                    fullScreenDialog.imageSource = previewImage.source
                                    fullScreenDialog.open()
                                }
                            }
                        }

                        Rectangle {
                            anchors.centerIn: parent
                            width: 100
                            height: 100
                            radius: 50
                            color: "#CC000000"
                            visible: dialog.isVideoPreview

                            Text {
                                anchors.centerIn: parent
                                text: "▶"
                                font.pixelSize: 50
                                color: "white"
                            }
                        }
                    }
                }
            }
        }

        onAccepted: {
            console.log("选中的文件： " + dialog.list_filename)
        }

        Dialog {
            id: fullScreenDialog
            width: 1920
            height: 800
            modal: true
            title: "全屏图像"

            Image {
                id: fullImage
                anchors.fill: parent
                source: ""
                fillMode: Image.PreserveAspectFit
            }

            Button {
                text: "关闭"
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    fullScreenDialog.close()
                }
            }

            onOpened: {
                fullImage.source = imageSource
            }

            property string imageSource: ""
        }
    }



    Dialog {
        id: yuyan
        title: "                   语言设置"
        width: 1000
        height: 600
        visible: false
        x:500
        y:300
        font {
            pixelSize: 50
        }

        standardButtons: Dialog.Ok | Dialog.Cancel

        background: Rectangle {
            color: "lightgray"
        }

        Column {
            spacing: 20
            anchors.centerIn: parent

            RadioButton {
                id: option123
                text: "       中文"
                font.pixelSize: 50
                checked: true
                onClicked: {


                        yuyan.title = "                   语言设置";
                        jifen.title = "                   积分设置";
                        banben.text = "版本";
                        yuyan_1.text = "语言";
                        textEdit6.text = "打开设置";
                        jifen_1.text = "积分";
                        textEdit5.text = "点击设置";
                        textEdit4.text = "点击导入";
                        taizhang_1.text = "台账功能";
                        textEdit3.text = "关";
                        ceju_1.text = "测距";
                        textEdit2.text = "点击切换";
                        text_jishu.text = "计数单位(/M)";
                        textEdit1.text = "打开设置";
                        quezhi_1.text = "阈值";
                        textEdit.text = "打开设置";
                        shijian_1.text = "时间设置";
                        wifi_blue_1.text = "WiFi/蓝牙";
                        wifiSwitch.text = "打开设置";
                        settingsDialog2222.title = "                                          设置";
                        quezhi_1.text = "阈值";
                        thresholdInputDialog.title = "设置阈值";
                        thresholdInputDialog_1.text = "输入阈值（0-10000）";
                        queding_2.text = "确定";
                        quxiao_2.text = "取消";
                        taizhang.title = "导入台账";
                        datetimeInputDialog.title = "输入日期和时间";
                        time_ok.text = "确定";
                        time_off.text = "取消";
                        dialog.title = "文件夹";
                        yanse.title = "                                        颜色设置";
                        option1.text = "红";
                        option2.text ="白";
                        option3.text ="蓝";
                        option4.text ="黄";
                        option5.text ="青";
                        option6.text ="品红";
                        option7.text ="玫红";
                        option8.text ="紫";
                        option9.text ="灰";
                        option10.text ="深绿";
                        option11.text ="淡蓝";
                        option12.text ="绿";
                    testtr.text = "UVIRSYS";
                    testtr.font.pixelSize = 65;
                    ebutton.setzywen(0);
                    ebutton.setyuyan(0);
                    dynamicText = ebutton.getzy();
                    updateModeText()
                }
            }

            RadioButton {
                id: option223
                text: "       English"
                font.pixelSize: 50
                onClicked: {


                        yuyan.title = "              Language settings";
                        jifen.title = "              Points setting";
                        banben.text = "version";
                        yuyan_1.text = "language";
                        textEdit6.text = "Open Settings";
                        jifen_1.text = "points";
                        textEdit5.text = "Click Settings";
                        textEdit4.text = "Click import";
                        taizhang_1.text = "Ledger function";
                        textEdit3.text = "off";
                        ceju_1.text = "ranging";
                        textEdit2.text = "Click switch";
                        text_jishu.text = "Counting unit(/M)"
                        textEdit1.text = "Open Settings";
                        quezhi_1.text = "threshold";
                        textEdit.text = "Open Settings";
                        shijian_1.text = "time setting";
                        wifi_blue_1.text = "  WiFi/Blue";
                        wifiSwitch.text = "Open Settings";
                        settingsDialog2222.title = "                                          set up";
                        quezhi_1.text = "threshold";
                        thresholdInputDialog.title = "Set threshold";
                        thresholdInputDialog_1.text = "Input threshold(0-10000)";
                        queding_2.text = "sure";
                        quxiao_2.text = "cancel";
                        taizhang.title = "Import ledger";
                        datetimeInputDialog.title = "Enter date and time";
                        time_ok.text = "sure";
                        time_off.text = "cancel";
                        dialog.title = "Folder";
                        yanse.title = "                                   color setting";
                        option1.text = "red";
                        option2.text ="white";
                        option3.text ="blue";
                        option4.text ="yellow";
                        option5.text ="blue or green";
                        option6.text ="magenta";
                        option7.text ="rose red";
                        option8.text ="purple";
                        option9.text ="ash";
                        option10.text ="dark green";
                        option11.text ="light blue";
                        option12.text ="green";

                    testtr.text = "UVIRSYS";
                    testtr.font.pixelSize = 60;
                    ebutton.setyuyan(1);
                    ebutton.setzywen(1);
                    dynamicText = ebutton.getzy();

                    updateModeText()
                }
            }
            RadioButton {
                id: option323
                text: "       Русский язык"
                font.pixelSize: 50
                // checked: true
                onClicked: {
                    ebutton.setzywen(2);

                        yuyan.title = "                   Русский язык";
                        jifen.title = "                   Настройки баллов";
                        banben.text = "Версия";
                        yuyan_1.text = "Язык";
                        textEdit6.text = "Открыть настройки";
                        jifen_1.text = "Интегрирование UV";
                        textEdit5.text = "Нажмите для настройки";
                        textEdit4.text = "Нажмите для импорта";
                        taizhang_1.text = "Функция учёта";
                        textEdit3.text = "off";
                        ceju_1.text = "Измерение расстояния";
                        textEdit2.text = "Нажмите для переключения";
                        text_jishu.text = "Единица измерения (/M)";
                        textEdit1.text = "Открыть настройки";
                        quezhi_1.text = "Сигнал";
                        textEdit.text = "Открыть настройки";
                        shijian_1.text = "Настройки времени";
                        wifi_blue_1.text = "WiFi/Bluetooth";
                        wifiSwitch.text = "Открыть настройки";
                        settingsDialog2222.title = "                                       Настройки";
                        thresholdInputDialog.title = "Установить сигнал ";
                        thresholdInputDialog_1.text = "Введите порог сигнала (0-10000)";
                        queding_2.text = "ОК";
                        quxiao_2.text = "Отмена";
                        taizhang.title = "Импорт учёта";
                        datetimeInputDialog.title = "Введите дату и время";
                        time_ok.text = "OK";
                        time_off.text = "Отмена";
                        dialog.title = "Папка";
                        yanse.title = "                                Настройки цвета";
                        option1.text = "Красный";
                        option2.text ="Белый";
                        option3.text ="Синий";
                        option4.text ="Жёлтый";
                        option5.text ="Бирюзовый";
                        option6.text ="Пурпурный";
                        option7.text ="Розовый";
                        option8.text ="Фиолетовый";
                        option9.text ="Серый";
                        option10.text ="Тёмно\n-зелёный";
                        option11.text ="Светло\n-синий";
                        option12.text ="Зелёный";

                    testtr.text = "UVIRSYS";
                    testtr.font.pixelSize = 65;
                    ebutton.setyuyan(2);
                    dynamicText = ebutton.getzy();
                    updateModeText()
                }
            }




            RadioButton {
                id: option3232
                text: "       日本語"
                font.pixelSize: 50
                // checked: true
                onClicked: {
                    listView.model.setProperty(0, "name", "レッド");
                    listView.model.setProperty(1, "name", "ホワイト");
                    listView.model.setProperty(2, "name", "ブルー");
                    listView.model.setProperty(3, "name", "イエロー");
                    listView.model.setProperty(4, "name", "スカイブルー");
                    listView.model.setProperty(5, "name", "マゼンタ");
                    listView.model.setProperty(6, "name", "ローズレッド");
                    listView.model.setProperty(7, "name", "パープル");
                    listView.model.setProperty(8, "name", "灰（はい）");
                    listView.model.setProperty(9, "name", "深緑");
                    listView.model.setProperty(10, "name", "ディープブルー");
                    listView.model.setProperty(11, "name", "グリーン");
                    listView1.model.setProperty(0, "name", "積分");
                    listView1.model.setProperty(1, "name", "カウント単位");
                    listView1.model.setProperty(2, "name", "しきい値");
                    listView1.model.setProperty(3, "name", "台帳機能");
                    listView1.model.setProperty(4, "name", "レンジスイッチ");
                    listView1.model.setProperty(5, "name", "wifi/Blue");
                    listView1.model.setProperty(6, "name", "言語");
                    listView1.model.setProperty(7, "name", "時刻設定");
                    listView1.model.setProperty(8, "name", "位置スイッチ");
                    factorCorrectionTitle.text = "係数補正";
                    shezhi_biaoti_1 = "設定";
                    yanseshezhi_biaoti_1 = "色設定";
                    testtr.text = "UVIRSYS";
                    //testtr.text = "武高电测";
                    //quezhi_1.text = "阈值";
                    thresholdInputDialog.title = "しきい値の設定";
                    thresholdInputDialog_1.text = "しきい値の入力（0～10000）";
                    queding_2.text = "確定";
                    quxiao_2.text = "キャンセル";
                    taizhang.title = "導入台帳";
                    datetimeInputDialog.title = "日付と時刻の入力";
                    time_ok.text = "確定";
                    time_off.text = "キャンセル";
                    dialog.title = "フォルダ";
                    fullScreenDialog.title = "全画面画像";
                    testtr.font.pixelSize = 65;
                    ebutton.setzywen(4);
                    ebutton.setyuyan(4);
                    dynamicText = ebutton.getzy();
                    ebutton.switch_l(1);
                    updateModeText()

                }
            }





        }
    }


    ApplicationWindow {
        id:yanse_jiemian
        objectName: "yanse"
        visible: false
        width: 1920
        height: 1080
        // 设置无边框窗口
        flags: Qt.FramelessWindowHint
        // 设置透明背景
        color: "transparent"
        // 初始化语言状态属性
        property bool isChinese: true
        // 当前选中的颜色索引
        property int currentColorIndex: -1

        // 主要布局
        Rectangle {
            width: parent.width
            height: parent.height
            color: "#e8e8e8"

            //顶部区域
            Rectangle {
                width: parent.width
                height: 140
                color: "#ffffff"
                z: 1

                // 返回按钮
                Image {
                    anchors.left: parent.left
                    anchors.leftMargin: 30
                    anchors.verticalCenter: parent.verticalCenter
                    source: "photo/fanhui.png"
                    width: 80
                    height: 80

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            yanse_jiemian.visible = false;
                        }
                    }
                }

                Text {
                    id: yanse_biaoti
                    anchors.centerIn: parent
                    color: "#000000"
                    text: qsTr(yanseshezhi_biaoti_1)
                    font.bold: true
                    font.pixelSize: 48
                }
            }

            Rectangle{
                id: settings_list_yanse

                // 颜色模型（语言切换时修改）
                Item {
                    id: listView
                    property var model: ListModel {
                        ListElement { name: "红" }
                        ListElement { name: "白" }
                        ListElement { name: "蓝" }
                        ListElement { name: "黄" }
                        ListElement { name: "青" }
                        ListElement { name: "品红" }
                        ListElement { name: "玫红" }
                        ListElement { name: "紫" }
                        ListElement { name: "灰" }
                        ListElement { name: "深绿" }
                        ListElement { name: "淡蓝" }
                        ListElement { name: "绿" }
                    }
                }

                visible: true
                anchors.left: parent.left
                anchors.leftMargin: 40
                anchors.right: parent.right
                anchors.rightMargin: 40
                anchors.top: parent.top
                anchors.topMargin: 160
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
                color: "#ffffff"
                radius: 30
                clip: true

                Flickable {
                    anchors.fill: parent
                    contentHeight: 12 * 150
                    boundsBehavior: Flickable.DragAndOvershootBounds

                    Column{
                        width: parent.width
                        Repeater{
                            model: listView.model
                            delegate:Rectangle{
                                width:parent.width
                                height:150
                                color: colorItem.pressed? "#f5f5f5" : (yanse_jiemian.currentColorIndex === index ? "#e8f5e9" : "#ffffff")
                                Behavior on color{
                                    ColorAnimation {
                                        duration: 100
                                    }
                                }
                                Rectangle{
                                    anchors.top: parent.top
                                    width:parent.width - 50
                                    height:1
                                    color:"#e8e8e8"
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    visible: index > 0
                                }
                                Row{
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 30
                                    spacing: 20

                                    Rectangle {
                                        width: 40
                                        height: 40
                                        radius: 20
                                        color: {
                                            var colors = ["#FF0000","#FFFFFF","#0000FF","#FFFF00","#00FFFF",
                                                         "#FF00FF","#FF1493","#800080","#808080","#006400",
                                                         "#ADD8E6","#008000"];
                                            return colors[index] || "#000000";
                                        }
                                        border.color: "#cccccc"
                                        border.width: 1
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                    Text {
                                        text: model.name
                                        font.pixelSize: 52
                                        color: "#000000"
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                }

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.right: parent.right
                                    anchors.rightMargin: 40
                                    text: yanse_jiemian.currentColorIndex === index ? "✓" : ""
                                    font.pixelSize: 48
                                    color: "#4CAF50"
                                }

                                MouseArea {
                                    id: colorItem
                                    anchors.fill: parent
                                    onClicked: {
                                        yanse_jiemian.currentColorIndex = index;
                                        ebutton.setmsqh(index+1);
                                        ebutton.writeDigitToFile("/opt/c_mv",index+1);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Dialog {

        id: jifen
        title: "                   积分设置"
        width: 1000
        height: 600
        visible: false
        x:500
        y:300
        font {
            pixelSize: 50
        }

        standardButtons: Dialog.Ok | Dialog.Cancel

        background: Rectangle {
            color: "white"
        }

        Column {
            spacing: 20
            anchors.centerIn: parent
            RadioButton {

                text: "       OFF"
                font.pixelSize: 50
                checked: true
                onClicked: {
                    ebutton.setjf(0);
                }
            }

            RadioButton {
                id: j1
                text: "       X2"
                font.pixelSize: 50
                onClicked: {
                    ebutton.setjf(1);
                }
            }

            RadioButton {
                id: j2
                text: "       X4"
                font.pixelSize: 50
                onClicked: {
                    ebutton.setjf(2);
                }
            }
            RadioButton {
                id: j3
                text: "       X8"
                font.pixelSize: 50
                onClicked: {
                    ebutton.setjf(3);
                }
            }
            RadioButton {
                id: j4
                text: "       X16"
                font.pixelSize: 50
                onClicked: {
                    ebutton.setjf(4);
                }
            }
        }
    }





    // 控制年份数字
    property int year: 2025 // 初始年份
    property int yue: 1 // 初始
    property int ri: 1 // 初始
    property int s: 1 // 初始
    property int f: 1 // 初始
    property int m: 1 // 初始
    ApplicationWindow {
        id:shezhi_jiemian
        objectName: "shezhi"
        visible: false
        width: 1920
        height: 1080
        // 设置无边框窗口
        flags: Qt.FramelessWindowHint
        // 设置透明背景
        color: "transparent"




        Dialog {
            id: thresholdInputDialog
            z:98
            objectName: "thresholdInputDialog"
            title: "设置阈值"
            width: 900
            height: 600
            //visible: true
            modal: true
            focus: true
            x:500
            y:300
            font.pixelSize: 45
            Column {
                spacing: 30
                anchors.centerIn: parent
                Label {
                    id:thresholdInputDialog_1
                    text: "输入阈值（0-10000）"
                }

                Row {
                    spacing: 20
                    z:99
                    ComboBox {
                        id: quezhi
                        width: 200
                        height: 60
                        z:99
                        model: ListModel {
                            ListElement { second: "0" }
                            ListElement { second: "1000" }
                            ListElement { second: "2000" }
                            ListElement { second: "3000" }
                            ListElement { second: "4000" }
                            ListElement { second: "5000" }
                            ListElement { second: "6000" }
                            ListElement { second: "7000" }
                            ListElement { second: "8000" }
                            ListElement { second: "9000" }
                            ListElement { second: "10000" }

                        }
                        popup {
                            z: 1000 // 让下拉框显示在Dialog之上
                        }
                        currentIndex:1
                        delegate: Item {
                            width: quezhi.width
                            height: 40

                            Rectangle {
                                width: parent.width
                                height: parent.height
                                color: "#f0f0f0"

                                Text {
                                    text: model.second
                                    anchors.centerIn: parent
                                    font.pixelSize: 45
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        quezhi.currentIndex = index;
                                        console.log(quezhi.currentText);
                                        quezhi.popup.close();

                                    }
                                }
                            }
                        }
                    }

                }

                Button {
                    id:queding_2
                    text: "确定"
                    onClicked: {
                        ebutton.setquezhi(quezhi.currentText);

                        thresholdInputDialog.close();

                    }
                }

                Button {
                    id:quxiao_2
                    text: "取消"
                    onClicked: thresholdInputDialog.close()
                }
                Button{
                    id:q_button
                    text: "OFF"
                    width: 200
                    height: 60
                    font.pixelSize: 30

                    background: Rectangle {
                        color: q_button.text === "ON" ? "#4CAF50" : "#f44336"
                        radius: 8
                        border.width: 1
                        border.color: "#ccc"
                    }
                    contentItem: Text {
                        text: q_button.text
                        color: "white"
                        font: q_button.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked:{
                        if(q_button.text === "OFF"){
                            q_button.text = "ON";
                            q_time.start();
                        }else
                        {
                            q_button.text = "OFF";
                            q_time.stop();
                            baojing.visible = false;
                            ebutton.stopAlarm();
                        }
                    }
                }
            }

        }


        Dialog {

            id: datetimeInputDialog
            objectName: "datetimeInputDialog"
            title: "输入日期和时间"
            z: 999  // 提高 z 值，确保在其他元素之上



            modal: true
            focus: true
            width: 1000
            height: 600
            x: 500
            y: 300
            visible: false
            font.pixelSize: 45
            Column {
                spacing: 20

                Label {
                    text: "YYYY-MM-DD HH:mm:ss"
                }

                Row {
                    spacing: 10



                    // 年份文本框
                    Item {
                        width: 180
                        height: 180

                        // 显示年份的文本
                        Text {
                            id: yearText
                            text: year
                            font.pixelSize: 45
                            anchors.centerIn: parent
                            font.family: "Arial"
                        }

                        // 上按钮
                        Button {
                            text: "+"
                            width: 40
                            height: 40
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right

                            onClicked: {
                                year += 1
                            }
                        }

                        // 下按钮
                        Button {
                            text: "-"
                            width: 40
                            height: 40
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right

                            onClicked: {
                                year -= 1
                            }
                        }


                    }

                    Label {
                        y:50
                        text: "-"
                    }

                    Item {
                        width: 120
                        height: 180

                        // 显示年份的文本
                        Text {
                            id: yueText
                            text: yue
                            font.pixelSize: 45
                            anchors.centerIn: parent
                            font.family: "Arial"
                        }

                        // 上按钮
                        Button {
                            text: "+"
                            width: 40
                            height: 40
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right

                            onClicked: {
                                if(yue < 12)
                                    yue += 1
                            }
                        }

                        // 下按钮
                        Button {
                            text: "-"
                            width: 40
                            height: 40
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right

                            onClicked: {
                                if(yue>1)
                                    yue -= 1
                            }
                        }


                    }



                    Label {
                        y:50
                        text: "-"
                    }
                    Item {
                        width: 120
                        height: 180

                        // 显示年份的文本
                        Text {
                            id: riText
                            text: ri
                            font.pixelSize: 45
                            anchors.centerIn: parent
                            font.family: "Arial"
                        }

                        // 上按钮
                        Button {
                            text: "+"
                            width: 40
                            height: 40
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right

                            onClicked: {
                                if(ri < 31)
                                    ri += 1
                            }
                        }

                        // 下按钮
                        Button {
                            text: "-"
                            width: 40
                            height: 40
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right

                            onClicked: {
                                if(ri>1)
                                    ri -= 1
                            }
                        }


                    }

                    Label {
                        y:50
                        text: "-"
                    }
                    Item {
                        width: 120
                        height: 180

                        // 显示年份的文本
                        Text {
                            id: sText
                            text: s
                            font.pixelSize: 45
                            anchors.centerIn: parent
                            font.family: "Arial"
                        }

                        // 上按钮
                        Button {
                            text: "+"
                            width: 40
                            height: 40
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right

                            onClicked: {
                                if(s < 23)
                                    s += 1
                            }
                        }

                        // 下按钮
                        Button {
                            text: "-"
                            width: 40
                            height: 40
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right

                            onClicked: {
                                if(s>0)
                                    s -= 1
                            }
                        }


                    }

                    Label {
                        y:50
                        text: "-"
                    }
                    Item {
                        width: 120
                        height: 180

                        // 显示年份的文本
                        Text {
                            id: fText
                            text: f
                            font.pixelSize: 45
                            anchors.centerIn: parent
                            font.family: "Arial"
                        }

                        // 上按钮
                        Button {
                            text: "+"
                            width: 40
                            height: 40
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right

                            onClicked: {
                                if(f < 59)
                                    f += 1
                            }
                        }

                        // 下按钮
                        Button {
                            text: "-"
                            width: 40
                            height: 40
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right

                            onClicked: {
                                if(f>0)
                                    f -= 1
                            }
                        }


                    }

                    Label {
                        y:50
                        text: "-"

                    }
                    Item {
                        width: 120
                        height: 180

                        // 显示年份的文本
                        Text {
                            id: mText
                            text: m
                            font.pixelSize: 45
                            anchors.centerIn: parent
                            font.family: "Arial"
                        }

                        // 上按钮
                        Button {
                            text: "+"
                            width: 40
                            height: 40
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right

                            onClicked: {
                                if(m < 59)
                                    m += 1
                            }
                        }

                        // 下按钮
                        Button {
                            text: "-"
                            width: 40
                            height: 40
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right

                            onClicked: {
                                if(m>0)
                                    m -= 1
                            }
                        }


                    }

                }
            }
            Button {
                id:time_ok
                            text: "确定"
                            x:200
                            y:350
                            width: 200
                            onClicked: {
                                times.stop();
                                //ebutton.settime(secondInput12.currentText,secondInput122.currentText,secondInput123.currentText,secondInput124.currentText,secondInput125.currentText,secondInput126.currentText);
                                ebutton.settime(year,yue,ri,s,f,m);
                                shijian = ebutton.gettime();
                                times.start();
                                datetimeInputDialog.close();
                                testtr1.visible = true;
                            }
                        }

                        Button {
                            id:time_off
                            text: "取消"
                            x:600
                            y:350
                            width: 200
                            onClicked: {
                                datetimeInputDialog.close();
                            }
                        }
        }

        Rectangle {
            id: taizhang
            visible: false
            width: 1920
            height: 1080
            color: "#e8e8e8"
            z: 100

            property int currentTab: 0  // 0=任务, 1=台账列表
            property string selectedFileName:""
            property string title: ""
            property string taskTabText: "任务"
            property string ledgerTabText: "台账列表"
            property string startTaskText: "启动任务"
            property string stopTaskText: "停止任务"
            property string currentTaskText: "当前任务"
            property string noFileText: "未选择"
            property string titleText: "台账管理"

            // 阻止点击穿透到底层设置界面
            MouseArea {
                anchors.fill: parent
            }

            FolderListModel {
                id: taizhangmulu
                folder: "file:///mnt/sdcard/Ledger storage"
                showDirs: true
                showDotAndDotDot: false
            }

            ListModel {
                id: contentModel
                objectName: "contentModel"
            }

            // 顶部白色栏
            Rectangle {
                width: parent.width
                height: 150
                color: "#ffffff"

                // 返回按钮
                Image {
                    anchors.left: parent.left
                    anchors.leftMargin: 30
                    anchors.verticalCenter: parent.verticalCenter
                    source: "photo/fanhui.png"
                    width: 80
                    height: 80

                    MouseArea {
                        anchors.fill: parent
                        onClicked: taizhang.visible = false
                    }
                }

                // 标题
                Text {
                    anchors.centerIn: parent
                    text: taizhang.titleText
                    font.pixelSize: 48
                    font.bold: true
                    color: "#000000"
                }
            }

            // 下方灰色区域
            Rectangle {
                anchors.top: parent.top
                anchors.topMargin: 150
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                color: "#e8e8e8"

                // 左侧按钮列
                Column {
                    anchors.left: parent.left
                    anchors.leftMargin: 30
                    anchors.top: parent.top
                    anchors.topMargin: 30
                    spacing: 20
                    width: 340

                    //当前任务
                    Rectangle{
                        width: 340
                        height: 180
                        radius: 12
                        color: "#ffffff"
                        border.width: 3
                        border.color: "#a8e6cf"

                        Text{
                            anchors.top: parent.top
                            anchors.topMargin: 14
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: taizhang.currentTaskText
                            font.pixelSize: 28
                            color: "#666666"
                        }

                        Rectangle{
                            anchors.top: parent.top
                            anchors.topMargin: 55
                            anchors.horizontalCenter:parent.horizontalCenter
                            width:parent.width -30
                            height:1
                            color:"#e0e0e0"
                        }
                        Text {
                            anchors.fill:parent
                            anchors.topMargin: 85
                            anchors.leftMargin:15
                            anchors.rightMargin: 15
                            text:taizhang.selectedFileName || taizhang.noFileText
                            font.pixelSize: 32
                            color:"#000000"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignHCenter
                            wrapMode: Text.WordWrap
                            maximumLineCount: 3
                            elide: Text.ElideRight
                        }
                    }

                    // 任务按钮
                    Rectangle {
                        id: taskBtn
                        width: 340
                        height: 120
                        radius: 12
                        color: taskMouse.pressed ? "#e8e8e8" : "#ffffff"
                        border.width: 4
                        border.color: taizhang.currentTab === 0 ? "#a8e6cf" : "#cccccc"
                        scale: taskMouse.pressed ? 0.95 : 1.0

                        Behavior on scale { NumberAnimation { duration: 100 } }
                        Behavior on color { ColorAnimation { duration: 100 } }

                        Text {
                            anchors.centerIn: parent
                            text: taizhang.taskTabText
                            font.pixelSize: 42
                            color: "#000000"
                        }

                        MouseArea {
                            id: taskMouse
                            anchors.fill: parent
                            onClicked: taizhang.currentTab = 0
                        }
                    }

                    // 台账列表按钮
                    Rectangle {
                        id: ledgerBtn
                        width: 340
                        height: 120
                        radius: 12
                        color: ledgerMouse.pressed ? "#e8e8e8" : "#ffffff"
                        border.width: 4
                        border.color: taizhang.currentTab === 1 ? "#a8d8ea" : "#cccccc"
                        scale: ledgerMouse.pressed ? 0.95 : 1.0

                        Behavior on scale { NumberAnimation { duration: 100 } }
                        Behavior on color { ColorAnimation { duration: 100 } }

                        Text {
                            anchors.centerIn: parent
                            text: taizhang.ledgerTabText
                            font.pixelSize: 42
                            color: "#000000"
                        }

                        MouseArea {
                            id: ledgerMouse
                            anchors.fill: parent
                            onClicked: taizhang.currentTab = 1
                        }
                    }

                    //启动任务按钮
                    Rectangle{
                        id: startTaskBtn
                        width: 340
                        height: 150
                        radius: 12
                        border.color: startTaskBtn.isTaskRunning ? "#ff9ea2" : "#a8e6cf"
                        property bool isChecked: false
                        property bool isTaskRunning: false
                        color:{
                            if (startTaskMouse.pressed)
                                return isChecked ? "#ff9ea2" : "#c8f0d4"
                            else
                                return isChecked ? "#ffbeb3" : "#d4f5e0"
                        }
                        border.width: 4
                        scale: startTaskMouse.pressed ? 0.95 : 1.0

                        // 缩放动画
                        Behavior on scale {
                            NumberAnimation { duration: 100 }
                        }
                        // 颜色过渡动画
                        Behavior on color {
                            ColorAnimation { duration: 100 }
                        }

                        Text {
                            anchors.centerIn:parent
                            text: startTaskBtn.isTaskRunning ? taizhang.stopTaskText : taizhang.startTaskText
                            font.pixelSize: 42
                            color: "#000000"
                        }
                        MouseArea{
                            id: startTaskMouse
                            anchors.fill: parent
                            onClicked: {
                                startTaskBtn.isChecked = !startTaskBtn.isChecked
                                startTaskBtn.isTaskRunning = !startTaskBtn.isTaskRunning
                                if (startTaskBtn.isTaskRunning) {
                                    taizhang.visible = false
                                    shezhi_jiemian.visible = false
                                    taskPanel.visible = true
                                    taskPanel.collapsed = false
                                } else {
                                    taskPanel.visible = false
                                }
                            }
                        }
                    }
                }

                // 右侧列表区域
                Rectangle {
                    anchors.left: parent.left
                    anchors.leftMargin: 400
                    anchors.right: parent.right
                    anchors.rightMargin: 30
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 10
                    color: "#ffffff"
                    radius: 10

                    // 任务列表
                    ListView {
                        id: taskListView
                        anchors.fill: parent
                        anchors.margins: 10
                        visible: taizhang.currentTab === 0
                        clip: true
                        model: contentModel

                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 90
                            color: "#ffffff"

                            Rectangle {
                                anchors.bottom: parent.bottom
                                width: parent.width
                                height: 1
                                color: "#e0e0e0"
                            }

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.leftMargin: 30
                                text: model.lineText
                                font.pixelSize: 36
                                color: "#000000"
                            }
                        }
                    }

                    // 台账文件列表
                    ListView {
                        id: ledgerListView
                        anchors.fill: parent
                        anchors.margins: 10
                        visible: taizhang.currentTab === 1
                        clip: true
                        model: taizhangmulu
                        property int selectedIndex: -1
                        delegate: Rectangle {
                            width: parent.width
                            height: 90
                            color: ledgerItemMouse.pressed ? "#d0d0d0"
                                         : (ledgerListView.selectedIndex === index ? "#c8e6c9" : "#ffffff")
                            Behavior on color { ColorAnimation { duration: 120 } }
                            Rectangle {
                                anchors.bottom: parent.bottom
                                width: parent.width
                                height: 1
                                color: "#e0e0e0"
                            }

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.leftMargin: 30
                                text: fileName
                                font.pixelSize: 36
                                color: "#000000"
                                width: parent.width - 40
                                elide: Text.ElideRight
                            }

                            MouseArea {
                                id:ledgerItemMouse
                                anchors.fill: parent
                                onClicked: {
                                    ledgerListView.selectedIndex = index
                                    taizhang.selectedFileName = fileName
                                    ebutton.taizhangdaoru("/mnt/sdcard/Ledger storage/" + fileName)
                                }
                            }
                        }
                    }
                }
            }
        }


        // 初始化语言状态属性
        property bool isChinese: true

        // 主要布局
        Rectangle {
            width: parent.width
            height: parent.height
            color: "#e8e8e8"

            //顶部区域
            Rectangle {
                width: parent.width
                height: 140
                color: "#ffffff"
                z: 1

                // 返回按钮
                Image {
                    anchors.left: parent.left
                    anchors.leftMargin: 30
                    anchors.verticalCenter: parent.verticalCenter
                    source: "photo/fanhui.png"
                    width: 80
                    height: 80

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            shezhi_jiemian.visible = false;
                            gps_jiemian.visible = false;
                            yuyan_jiemian.visible = false;
                            jifen_page.visible = false;
                            jishu_page.visible = false;
                            ceju_page.visible = false;
                            yuyan_page.visible = false;
                            quezhi_page.visible = false;
                            wifi_page.visible = false;
                            shijian_page.visible = false;
                        }
                    }
                }

                Text {
                    id: shezhi_biaoti
                    anchors.centerIn: parent
                    color: "#000000"
                    text: qsTr(shezhi_biaoti_1)
                    font.bold: true
                    font.pixelSize: 48
                }
            }



            Item{
                id:listView1
                property var model: ListModel {
                    ListElement { name: "积分"; icon: "★" }
                    ListElement { name: "计数单位"; icon: "☰" }
                    ListElement { name: "阈值"; icon: "⚡" }
                    ListElement { name: "台账功能"; icon: "☷" }
                    ListElement { name: "测距开关"; icon: "◎" }
                    ListElement { name: "wifi/蓝牙"; icon: "◉" }
                    ListElement { name: "语言"; icon: "☎" }
                    ListElement { name: "时间设置"; icon: "◷" }
                    ListElement { name: "定位开关"; icon: "✈" }
                    //ListElement { name: "因子矫正"; icon: "⚙" }
                }
            }

            Rectangle{
                id: settings_list
                visible: !jifen_page.visible && !jishu_page.visible && !ceju_page.visible && !yuyan_page.visible && !quezhi_page.visible && !wifi_page.visible && !shijian_page.visible && !gps_jiemian.visible && !yinzijiaozheng_page.visible
                anchors.left: parent.left
                anchors.leftMargin: 40
                anchors.right: parent.right
                anchors.rightMargin: 40
                anchors.top: parent.top
                anchors.topMargin: 160 // 给返回按钮留出空间
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
                color: "#ffffff"
                radius: 30
                clip: true

                Flickable {
                    anchors.fill: parent
                    contentHeight: 10 * 150
                    boundsBehavior: Flickable.DragAndOvershootBounds

                    Column{
                        width: parent.width
                        Repeater{
                            model: listView1.model
                            delegate:Rectangle{
                                width:parent.width
                                height:150
                                color:itemArea.pressed? "#f5f5f5" : "#ffffff"
                                Behavior on color{
                                    ColorAnimation {
                                        duration: 100
                                    }
                                }
                                Rectangle{
                                    anchors.top: parent.top
                                    width:parent.width - 50
                                    height:1
                                    color:"#e8e8e8"
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    /*?*/
                                    visible: index > 0
                                }
                                Row{
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 30
                                    spacing: 20

                                    Text {
                                            text: model.icon
                                            font.pixelSize: 48
                                            color: "#888888"
                                            anchors.verticalCenter: parent.verticalCenter
                                        }
                                        Text {
                                            text: model.name
                                            font.pixelSize: 52
                                            color: "#000000"
                                            anchors.verticalCenter: parent.verticalCenter
                                        }
                                }


                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.right: parent.right
                                    anchors.rightMargin: 40
                                    text: "\u203A"   // 转义 >
                                    font.pixelSize: 52
                                    color: "#bbbbbb"
                                }

                                MouseArea {
                                        id: itemArea
                                        anchors.fill: parent
                                        onClicked: {
                                            if (index == 0) jifen_page.visible = true;
                                            if (index == 1) jishu_page.visible = true;
                                            if (index == 2) quezhi_page.visible = true;
                                            if (index == 3) taizhang.visible = true;
                                            if (index == 4) ceju_page.visible = true;
                                            if (index == 5) wifi_page.visible = true;
                                            if (index == 6) yuyan_page.visible = true;
                                            if (index == 7) {
                                                year = ebutton.getnnnn();
                                                yue = ebutton.getyy();
                                                ri = ebutton.getday();
                                                s = ebutton.gets();
                                                f = ebutton.getf();
                                                m = ebutton.getm();
                                                shijian_page.visible = true;
                                            }
                                            if(index == 8) {
                                                gps_jiemian.visible = true;
                                                //GPS初始化

                                            }
                                            if(index == 9) {
                                                yinzijiaozheng_page.visible = true;
                                            }
                                        }
                                }
                            }
                        }
                    }
                }
            }

            // ========== 积分子页面 ==========
                Rectangle {
                    id: jifen_page
                    anchors.fill: parent
                    color: "#e8e8e8"
                    visible: false
                    z: 10

                    // 顶部标题栏
                    Rectangle {
                        width: parent.width
                        height: 140
                        color: "#ffffff"

                        Image {
                            anchors.left: parent.left
                            anchors.leftMargin: 30
                            anchors.verticalCenter: parent.verticalCenter
                            source: "photo/fanhui.png"
                            width: 80
                            height: 80
                            MouseArea {
                                anchors.fill: parent
                                onClicked: jifen_page.visible = false
                            }
                        }

                        Text {
                            id:pointTitle
                            anchors.centerIn: parent
                            color: "#000000"
                            text: "积分"
                            font.bold: true
                            font.pixelSize: 48
                        }
                    }

                    // 内容区域
                    Rectangle {
                        anchors.left: parent.left
                        anchors.leftMargin: 40
                        anchors.right: parent.right
                        anchors.rightMargin: 40
                        anchors.top: parent.top
                        anchors.topMargin: 160
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 20
                        color: "#ffffff"
                        radius: 30

                        ListView {
                            id: listView_jifen_nei
                            anchors.fill: parent
                            anchors.margins: 20

                            model: ListModel {
                                ListElement { name: "OFF" }
                                ListElement { name: "X2" }
                                ListElement { name: "X4" }
                                ListElement { name: "X8" }
                                ListElement { name: "X16" }
                            }

                            property int currentIndex: 0

                            delegate: Rectangle {
                                width: parent.width
                                height: 120
                                color: "transparent"

                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 30
                                    spacing: 20

                                    RadioButton {
                                        scale: 2.5
                                        checked: listView_jifen_nei.currentIndex === index
                                        onClicked: {
                                            listView_jifen_nei.currentIndex = index
                                            ebutton.setjf(index)
                                            jifen_page.visible = false
                                        }
                                    }

                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: model.name
                                        color: "#333333"
                                        font.pixelSize: 36
                                    }
                                }

                                Rectangle {
                                    width: parent.width - 60
                                    height: 1
                                    color: "#e0e0e0"
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                            }
                        }
                    }
                }

                // ========== 计数单位子页面 ==========
                Rectangle {
                    id: jishu_page
                    anchors.fill: parent
                    color: "#e8e8e8"
                    visible: false
                    z: 10

                    Rectangle {
                        width: parent.width
                        height: 140
                        color: "#ffffff"

                        Image {
                            anchors.left: parent.left
                            anchors.leftMargin: 30
                            anchors.verticalCenter: parent.verticalCenter
                            source: "photo/fanhui.png"
                            width: 80
                            height: 80
                            MouseArea {
                                anchors.fill: parent
                                onClicked: jishu_page.visible = false
                            }
                        }

                        Text {
                            id:countTitle
                            anchors.centerIn: parent
                            color: "#000000"
                            text: "计数单位"
                            font.bold: true
                            font.pixelSize: 48
                        }
                    }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.leftMargin: 40
                        anchors.right: parent.right
                        anchors.rightMargin: 40
                        anchors.top: parent.top
                        anchors.topMargin: 160
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 20
                        color: "#ffffff"
                        radius: 30

                        ListView {
                            id: listView_jishu_nei
                            anchors.fill: parent
                            anchors.margins: 20

                            model: ListModel {
                                ListElement { name: "/M" }
                                ListElement { name: "/S" }
                            }

                            property int currentIndex: 0

                            delegate: Rectangle {
                                width: parent.width
                                height: 120
                                color: "transparent"

                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 30
                                    spacing: 20

                                    RadioButton {
                                        scale: 2.5
                                        checked: listView_jishu_nei.currentIndex === index
                                        onClicked: {
                                            listView_jishu_nei.currentIndex = index
                                            ebutton.xxxxxjishu(index)
                                            jishu_page.visible = false
                                        }
                                    }

                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: model.name
                                        color: "#333333"
                                        font.pixelSize: 36
                                    }
                                }

                                Rectangle {
                                    width: parent.width - 60
                                    height: 1
                                    color: "#e0e0e0"
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                            }
                        }
                    }
                }

                // ========== 测距开关子页面 ==========
                Rectangle {
                    id: ceju_page
                    anchors.fill: parent
                    color: "#e8e8e8"
                    visible: false
                    z: 10

                    Rectangle {
                        width: parent.width
                        height: 140
                        color: "#ffffff"

                        Image {
                            anchors.left: parent.left
                            anchors.leftMargin: 30
                            anchors.verticalCenter: parent.verticalCenter
                            source: "photo/fanhui.png"
                            width: 80
                            height: 80
                            MouseArea {
                                anchors.fill: parent
                                onClicked: ceju_page.visible = false
                            }
                        }

                        Text {
                            id:distanceTitle
                            anchors.centerIn: parent
                            color: "#000000"
                            text: "测距开关"
                            font.bold: true
                            font.pixelSize: 48
                        }
                    }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.leftMargin: 40
                        anchors.right: parent.right
                        anchors.rightMargin: 40
                        anchors.top: parent.top
                        anchors.topMargin: 160
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 20
                        color: "#ffffff"
                        radius: 30

                        ListView {
                            id: listView_ceju_nei
                            anchors.fill: parent
                            anchors.margins: 20

                            model: ListModel {
                                ListElement { name: "ON" }
                                ListElement { name: "OFF" }
                            }

                            property int currentIndex: 1

                            delegate: Rectangle {
                                width: parent.width
                                height: 120
                                color: "transparent"

                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 30
                                    spacing: 20

                                    RadioButton {
                                        scale: 2.5
                                        checked: listView_ceju_nei.currentIndex === index
                                        onClicked: {
                                            listView_ceju_nei.currentIndex = index
                                            ebutton.settjiguang(index)
                                            ceju_page.visible = false
                                        }
                                    }

                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: model.name
                                        color: "#333333"
                                        font.pixelSize: 36
                                    }
                                }

                                Rectangle {
                                    width: parent.width - 60
                                    height: 1
                                    color: "#e0e0e0"
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                            }
                        }
                    }
                }

                // ========== GPS开关子页面 ==========
                Rectangle {
                    id: gps_jiemian
                    anchors.fill: parent
                    color: "#e8e8e8"
                    visible: false
                    z: 10

                    Rectangle {
                        width: parent.width
                        height: 140
                        color: "#ffffff"

                        Image {
                            anchors.left: parent.left
                            anchors.leftMargin: 30
                            anchors.verticalCenter: parent.verticalCenter
                            source: "photo/fanhui.png"
                            width: 80
                            height: 80
                            MouseArea {
                                anchors.fill: parent
                                onClicked: gps_jiemian.visible = false
                            }
                        }

                        Text {
                            id:positionTitle
                            anchors.centerIn: parent
                            color: "#000000"
                            text: "Positioning Switch"
                            font.bold: true
                            font.pixelSize: 48
                        }
                    }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.leftMargin: 40
                        anchors.right: parent.right
                        anchors.rightMargin: 40
                        anchors.top: parent.top
                        anchors.topMargin: 160
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 20
                        color: "#ffffff"
                        radius: 30

                        ListView {
                            id: listView_gps
                            anchors.fill: parent
                            anchors.margins: 20

                            model: ListModel {
                                ListElement { name: "ON" }
                                ListElement { name: "OFF" }
                            }

                            property int currentIndex: 1

                            delegate: Rectangle {
                                width: parent.width
                                height: 120
                                color: "transparent"

                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 30
                                    spacing: 20

                                    RadioButton {
                                        scale: 2.5
                                        checked: listView_gps.currentIndex === index
                                        onClicked: {
                                            listView_gps.currentIndex = index
                                            gps_jiemian.visible = false
                                            ebutton.writeDigitToFile("/opt/gps_mv", index)
                                            if(index == 0){
                                                gps.visible = true
                                            }
                                            else
                                                gps.visible = false
                                        }
                                    }

                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: model.name
                                        color: "#333333"
                                        font.pixelSize: 36
                                    }
                                }

                                Rectangle {
                                    width: parent.width - 60
                                    height: 1
                                    color: "#e0e0e0"
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                            }
                        }
                    }
                }

                // ========== 因子矫正子页面 ==========
                Rectangle {
                    id: yinzijiaozheng_page
                    anchors.fill: parent
                    color: "#e8e8e8"
                    visible: false
                    z: 10

                    // 顶部标题栏
                    Rectangle {
                        width: parent.width
                        height: 140
                        color: "#ffffff"

                        Image {
                            anchors.left: parent.left
                            anchors.leftMargin: 30
                            anchors.verticalCenter: parent.verticalCenter
                            source: "photo/fanhui.png"
                            width: 80
                            height: 80
                            MouseArea {
                                anchors.fill: parent
                                onClicked: yinzijiaozheng_page.visible = false
                            }
                        }

                        Text {
                            id: factorCorrectionTitle
                            anchors.centerIn: parent
                            color: "#000000"
                            text: "因子矫正"
                            font.bold: true
                            font.pixelSize: 48
                        }
                    }

                    // 内容区域
                    Rectangle {
                        anchors.left: parent.left
                        anchors.leftMargin: 40
                        anchors.right: parent.right
                        anchors.rightMargin: 40
                        anchors.top: parent.top
                        anchors.topMargin: 160
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 20
                        color: "#ffffff"
                        radius: 30

                        // 因子矫正通用组件
                        Component {
                            id: factorFieldComp

                            Column {
                                spacing: 8
                                anchors.verticalCenter: parent.verticalCenter

                                // + 按钮
                                Rectangle {
                                    width: 80
                                    height: 80
                                    radius: 40
                                    color: maUpFactor.pressed ? "#e0e0e0" : "#f5f5f5"
                                    border.color: "#dddddd"
                                    border.width: 2
                                    anchors.horizontalCenter: parent.horizontalCenter

                                    Text {
                                        text: "+"
                                        font.pixelSize: 40
                                        color: "#666666"
                                        anchors.centerIn: parent
                                    }

                                    MouseArea {
                                        id: maUpFactor
                                        anchors.fill: parent
                                        onClicked: {
                                            control.onIncrement()
                                            control.value = control.getter()
                                        }
                                    }
                                }

                                // 数值显示
                                Rectangle {
                                    width: 160
                                    height: 100
                                    radius: 16
                                    color: "#fafafa"
                                    border.color: "#e0e0e0"
                                    border.width: 2

                                    Text {
                                        anchors.centerIn: parent
                                        text: control.value
                                        font.pixelSize: 52
                                        font.bold: true
                                        color: "#333333"
                                    }
                                }

                                // - 按钮
                                Rectangle {
                                    width: 80
                                    height: 80
                                    radius: 40
                                    color: maDownFactor.pressed ? "#e0e0e0" : "#f5f5f5"
                                    border.color: "#dddddd"
                                    border.width: 2
                                    anchors.horizontalCenter: parent.horizontalCenter

                                    Text {
                                        text: "−"
                                        font.pixelSize: 40
                                        color: "#666666"
                                        anchors.centerIn: parent
                                    }

                                    MouseArea {
                                        id: maDownFactor
                                        anchors.fill: parent
                                        onClicked: {
                                            control.onDecrement()
                                            control.value = control.getter()
                                        }
                                    }
                                }

                                // 标签
                                Text {
                                    text: control.label
                                    font.pixelSize: 28
                                    color: "#999999"
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    horizontalAlignment: Text.AlignHCenter
                                    width: 200
                                    wrapMode: Text.WordWrap
                                }
                            }
                        }

                        // 四个因子排列：上下两行，每行两个
                        Grid {
                            anchors.centerIn: parent
                            columns: 2
                            spacing: 40

                            // 水平距离
                            Loader {
                                sourceComponent: factorFieldComp
                                property var control: ({
                                    value: ebutton.getHorzDistCorrectionFactor(),
                                    label: "水平距离",
                                    getter: function() { return ebutton.getHorzDistCorrectionFactor() },
                                    onIncrement: function() {
                                        var v = ebutton.getHorzDistCorrectionFactor()
                                        ebutton.setHorzDistCorrectionFactor(v + 1)
                                    },
                                    onDecrement: function() {
                                        var v = ebutton.getHorzDistCorrectionFactor()
                                        ebutton.setHorzDistCorrectionFactor(v - 1)
                                    }
                                })
                            }

                            // 垂直距离
                            Loader {
                                sourceComponent: factorFieldComp
                                property var control: ({
                                    value: ebutton.getVertDistCorrectionFactor(),
                                    label: "垂直距离",
                                    getter: function() { return ebutton.getVertDistCorrectionFactor() },
                                    onIncrement: function() {
                                        var v = ebutton.getVertDistCorrectionFactor()
                                        ebutton.setVertDistCorrectionFactor(v + 1)
                                    },
                                    onDecrement: function() {
                                        var v = ebutton.getVertDistCorrectionFactor()
                                        ebutton.setVertDistCorrectionFactor(v - 1)
                                    }
                                })
                            }

                            // 水平角度
                            Loader {
                                sourceComponent: factorFieldComp
                                property var control: ({
                                    value: ebutton.getHorzAngCorrectionFactor(),
                                    label: "水平角度",
                                    getter: function() { return ebutton.getHorzAngCorrectionFactor() },
                                    onIncrement: function() {
                                        var v = ebutton.getHorzAngCorrectionFactor()
                                        ebutton.setHorzAngCorrectionFactor(v + 1)
                                    },
                                    onDecrement: function() {
                                        var v = ebutton.getHorzAngCorrectionFactor()
                                        ebutton.setHorzAngCorrectionFactor(v - 1)
                                    }
                                })
                            }

                            // 垂直角度
                            Loader {
                                sourceComponent: factorFieldComp
                                property var control: ({
                                    value: ebutton.getVertAngCorrectionFactor(),
                                    label: "垂直角度",
                                    getter: function() { return ebutton.getVertAngCorrectionFactor() },
                                    onIncrement: function() {
                                        var v = ebutton.getVertAngCorrectionFactor()
                                        ebutton.setVertAngCorrectionFactor(v + 1)
                                    },
                                    onDecrement: function() {
                                        var v = ebutton.getVertAngCorrectionFactor()
                                        ebutton.setVertAngCorrectionFactor(v - 1)
                                    }
                                })
                            }
                        }
                    }
                }

                // ========== 语言子页面 ==========
                Rectangle {
                    id: yuyan_page
                    anchors.fill: parent
                    color: "#e8e8e8"
                    visible: false
                    z: 10

                    Rectangle {
                        width: parent.width
                        height: 140
                        color: "#ffffff"

                        Image {
                            anchors.left: parent.left
                            anchors.leftMargin: 30
                            anchors.verticalCenter: parent.verticalCenter
                            source: "photo/fanhui.png"
                            width: 80
                            height: 80
                            MouseArea {
                                anchors.fill: parent
                                onClicked: yuyan_page.visible = false
                            }
                        }

                        Text {
                            id:languageTitle
                            anchors.centerIn: parent
                            color: "#000000"
                            text: "语言"
                            font.bold: true
                            font.pixelSize: 48
                        }
                    }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.leftMargin: 40
                        anchors.right: parent.right
                        anchors.rightMargin: 40
                        anchors.top: parent.top
                        anchors.topMargin: 160
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 20
                        color: "#ffffff"
                        radius: 30

                        ListView {
                            id: listView_yuyan_nei
                            anchors.fill: parent
                            anchors.margins: 20

                            model: ListModel {
                                ListElement { name: "中文" }
                                ListElement { name: "English" }
                                ListElement { name: "Русский" }
                                ListElement { name: "日本語" }
                                ListElement { name: "Italiano" }
                            }

                            property int currentIndex: 0

                            delegate: Rectangle {
                                width: parent.width
                                height: 120
                                color: "transparent"

                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 30
                                    spacing: 20

                                    RadioButton {
                                        scale: 2.5
                                        checked: listView_yuyan_nei.currentIndex === index
                                        onClicked: {
                                            listView_yuyan_nei.currentIndex = index
                                            if(index == 0)
                                            {
                                                console.log("cn")
                                                cn_l.running = true;

                                            }else if(index == 1) {
                                                en_l.running = true;
                                                console.log("en")
                                            }else if(index == 3) {
                                                rn_l.running = true;
                                                console.log("jp")
                                            }else if(index == 4){
                                                ydl_l.running = true;
                                                console.log("itl")
                                            }
                                            else {
                                                eyun_l.running = true
                                                console.log("rua")
                                            }
                                            yuyan_page.visible = false
                                        }
                                    }

                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: model.name
                                        color: "#333333"
                                        font.pixelSize: 36
                                    }
                                }

                                Rectangle {
                                    width: parent.width - 60
                                    height: 1
                                    color: "#e0e0e0"
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                            }
                        }
                    }
                }
        }

        // ========== 阈值子页面 ==========
        Rectangle {
            id: quezhi_page
            anchors.fill: parent
            color: "#e8e8e8"
            visible: false
            z: 10

            Rectangle {
                width: parent.width
                height: 140
                color: "#ffffff"

                Image {
                    anchors.left: parent.left
                    anchors.leftMargin: 30
                    anchors.verticalCenter: parent.verticalCenter
                    source: "photo/fanhui.png"
                    width: 80
                    height: 80
                    MouseArea {
                        anchors.fill: parent
                        onClicked: quezhi_page.visible = false
                    }
                }

                Text {
                    id: thresholdTitle
                    anchors.centerIn: parent
                    color: "#000000"
                    text: "阈值"
                    font.bold: true
                    font.pixelSize: 48
                }
            }

            Rectangle {
                anchors.left: parent.left
                anchors.leftMargin: 40
                anchors.right: parent.right
                anchors.rightMargin: 40
                anchors.top: parent.top
                anchors.topMargin: 160
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
                color: "#ffffff"
                radius: 30

                Column {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 10

                    // ON/OFF 开关按钮
                    Rectangle {
                        width: parent.width
                        height: 80
                        color: "transparent"

                        Row {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 30
                            spacing: 20

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: "阈值开关"
                                color: "#333333"
                                font.pixelSize: 36
                            }

                            Button {
                                id: quezhi_switch_btn
                                text: "OFF"
                                width: 140
                                height: 60
                                font.pixelSize: 30

                                background: Rectangle {
                                    color: quezhi_switch_btn.text === "ON" ? "#4CAF50" : "#f44336"
                                    radius: 8
                                    border.width: 1
                                    border.color: "#888"
                                }
                                contentItem: Text {
                                    text: quezhi_switch_btn.text
                                    color: "white"
                                    font: quezhi_switch_btn.font
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                onClicked: {
                                    if (quezhi_switch_btn.text === "OFF") {
                                        quezhi_switch_btn.text = "ON"
                                        ebutton.setquezhi_sw(1)
                                        q_time.start()
                                    } else {
                                        quezhi_switch_btn.text = "OFF"
                                        ebutton.setquezhi_sw(0)
                                        q_time.stop()
                                        baojing.visible = false
                                    }
                                }
                            }
                        }

                        Rectangle {
                            width: parent.width - 60
                            height: 1
                            color: "#e0e0e0"
                            anchors.bottom: parent.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }

                    ListView {
                        id: listView_quezhi_nei
                        width: parent.width
                        height: parent.height - 90

                        model: ListModel {
                            ListElement { name: "0" }
                            ListElement { name: "1000" }
                            ListElement { name: "2000" }
                            ListElement { name: "3000" }
                            ListElement { name: "4000" }
                            ListElement { name: "5000" }
                            ListElement { name: "6000" }
                            ListElement { name: "7000" }
                            ListElement { name: "8000" }
                            ListElement { name: "9000" }
                            ListElement { name: "10000" }
                        }

                        property int currentIndex: 1

                        delegate: Rectangle {
                            width: parent.width
                            height: 120
                            color: "transparent"

                            Row {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.leftMargin: 30
                                spacing: 20

                                RadioButton {
                                    scale: 2.5
                                    checked: listView_quezhi_nei.currentIndex === index
                                    onClicked: {
                                        listView_quezhi_nei.currentIndex = index
                                        quezhi.currentIndex = index
                                        ebutton.setquezhi(index * 1000)
                                        quezhi_page.visible = false
                                    }
                                }

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: model.name
                                    color: "#333333"
                                    font.pixelSize: 36
                                }
                            }

                            Rectangle {
                                width: parent.width - 60
                                height: 1
                                color: "#e0e0e0"
                                anchors.bottom: parent.bottom
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                }
            }
        }

                // ========== WiFi/蓝牙设置子页面 ==========
                Rectangle {
                    id: wifi_page
                    anchors.fill: parent
                    color: "#e8e8e8"
                    visible: false
                    z: 10

                    // 标题栏
                    Rectangle {
                        width: parent.width
                        height: 140
                        color: "#ffffff"

                        Image {
                            anchors.left: parent.left
                            anchors.leftMargin: 30
                            anchors.verticalCenter: parent.verticalCenter
                            source: "photo/fanhui.png"
                            width: 80
                            height: 80
                            MouseArea {
                                anchors.fill: parent
                                onClicked: wifi_page.visible = false
                            }
                        }

                        Text {
                            id:wifiBtTitle
                            anchors.centerIn: parent
                            color: "#000000"
                            text: "WiFi/蓝牙"
                            font.bold: true
                            font.pixelSize: 48
                        }
                    }

                    // 左右两栏布局
                    Row {
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.right: parent.right
                        anchors.rightMargin: 20
                        anchors.top: parent.top
                        anchors.topMargin: 160
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 20
                        spacing: 20

                        // ========== 左侧：WiFi ==========
                        Rectangle {
                            width: parent.width / 2 - 10
                            height: parent.height
                            color: "#ffffff"
                            radius: 20

                            Column {
                                anchors.fill: parent
                                anchors.margins: 20
                                spacing: 10

                                Text {
                                    text: "WiFi"
                                    font.pixelSize: 40
                                    font.bold: true
                                    color: "#333333"
                                }

                                // WiFi 开关
                                Row {
                                    spacing: 15
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: wifiEnabled ? "ON" : "OFF"
                                        font.pixelSize: 32
                                        color: wifiEnabled ? "#4CAF50" : "#999999"
                                        font.bold: true
                                    }
                                    Rectangle {
                                        width: 80; height: 40; radius: 20
                                        color: wifiEnabled ? "#4CAF50" : "#cccccc"
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                wifiEnabled = !wifiEnabled
                                                if (wifiEnabled) {
                                                    wifiStatusText = "扫描中..."
                                                    wifiScanTimer.start()
                                                } else {
                                                    wifiStatusText = ""
                                                    wifiSSIDList.clear()
                                                    connectedSSID = ""
                                                    ebutton.stopFileServer()
                                                    fileServerRunning = false
                                                    fileServerURL = ""
                                                }
                                            }
                                        }
                                    }
                                }

                                // 已连接状态
                                Text {
                                    visible: connectedSSID !== ""
                                    text: "已连接: " + connectedSSID
                                    font.pixelSize: 28
                                    color: "#4CAF50"
                                    font.bold: true
                                    wrapMode: Text.Wrap
                                    width: parent.width
                                }

                                // 连接失败提示
                                Text {
                                    visible: wifiStatusText !== "" && connectedSSID === "" && !wifiScanning
                                    text: wifiStatusText
                                    font.pixelSize: 26
                                    color: "#ff4444"
                                    wrapMode: Text.Wrap
                                    width: parent.width
                                }

                                // ===== 文件共享 =====
                                Rectangle {
                                    visible: connectedSSID !== ""
                                    width: parent.width - 20
                                    height: fileServerRunning ? 140 : 70
                                    color: "#e3f2fd"
                                    radius: 12
                                    Column {
                                        anchors.fill: parent
                                        anchors.margins: 14
                                        spacing: 10
                                        Row {
                                            spacing: 12
                                            Text {
                                                text: "文件共享"
                                                font.pixelSize: 24
                                                font.bold: true
                                                color: "#1565C0"
                                                anchors.verticalCenter: parent.verticalCenter
                                            }
                                            Rectangle {
                                                width: fileServerRunning ? 90 : 110
                                                height: 38
                                                radius: 8
                                                color: fileServerRunning ? "#f44336" : "#4CAF50"
                                                Text {
                                                    anchors.centerIn: parent
                                                    text: fileServerRunning ? "停止" : "启动共享"
                                                    font.pixelSize: 18
                                                    color: "#ffffff"
                                                }
                                                MouseArea {
                                                    anchors.fill: parent
                                                    onClicked: {
                                                        if (!fileServerRunning) {
                                                            var ip = ebutton.startFileServer()
                                                            if (ip !== "") {
                                                                fileServerURL = "http://" + ip + ":8080"
                                                                fileServerRunning = true
                                                            } else {
                                                                wifiStatusText = "文件服务器启动失败"
                                                            }
                                                        } else {
                                                            ebutton.stopFileServer()
                                                            fileServerRunning = false
                                                            fileServerURL = ""
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        Text {
                                            visible: fileServerRunning
                                            text: "手机浏览器访问:"
                                            font.pixelSize: 18
                                            color: "#666666"
                                        }
                                        Text {
                                            visible: fileServerRunning
                                            text: fileServerURL
                                            font.pixelSize: 22
                                            color: "#1565C0"
                                            font.bold: true
                                            font.italic: true
                                        }
                                    }
                                }

                                // WiFi 列表
                                Text {
                                    visible: wifiEnabled
                                    text: "可用WiFi列表:"
                                    font.pixelSize: 30
                                    font.bold: true
                                    color: "#666666"
                                }

                                ListView {
                                    id: wifiListView
                                    visible: wifiEnabled
                                    width: parent.width
                                    height: parent.height - 250
                                    clip: true
                                    model: ListModel { id: wifiSSIDList }
                                    delegate: Rectangle {
                                        width: wifiListView.width
                                        height: 60
                                        color: index % 2 === 0 ? "#f5f5f5" : "#ffffff"
                                        radius: 6
                                        Row {
                                            anchors.verticalCenter: parent.verticalCenter
                                            anchors.left: parent.left
                                            anchors.leftMargin: 15
                                            spacing: 10
                                            Text {
                                                text: "  " + modelData
                                                font.pixelSize: 28
                                                color: "#333333"
                                                anchors.verticalCenter: parent.verticalCenter
                                            }
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                selectedSSID = modelData
                                                passwordInput.text = ""
                                                wifiPasswordPopup.visible = true
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // ========== 右侧：蓝牙 ==========
                        Rectangle {
                            width: parent.width / 2 - 10
                            height: parent.height
                            color: "#ffffff"
                            radius: 20

                            Column {
                                anchors.fill: parent
                                anchors.margins: 20
                                spacing: 10

                                Text {
                                    id: btTitle
                                    text: "蓝牙"
                                    font.pixelSize: 40
                                    font.bold: true
                                    color: "#333333"
                                }

                                // 蓝牙开关 + 扫描按钮
                                Row {
                                    spacing: 20
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: btEnabled ? "ON" : "OFF"
                                        font.pixelSize: 32
                                        color: btEnabled ? "#2196F3" : "#999999"
                                        font.bold: true
                                    }
                                    Rectangle {
                                        width: 80; height: 40; radius: 20
                                        color: btEnabled ? "#2196F3" : "#cccccc"
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                btEnabled = !btEnabled
                                                if (btEnabled) {
                                                    btStatusText = "正在检测蓝牙..."
                                                    btScanTimer.start()
                                                } else {
                                                    // 关闭蓝牙：断开当前连接
                                                    if (connectedBtAddr !== "") {
                                                        ebutton.bluetoothDisconnect(connectedBtAddr)
                                                    }
                                                    btStatusText = ""
                                                    btDeviceList.clear()
                                                    btConnectedDevice = ""
                                                    connectedBtAddr = ""
                                                }
                                            }
                                        }
                                    }
                                    // 扫描按钮
                                    Rectangle {
                                        width: 100; height: 42; radius: 10
                                        color: btScanning ? "#ff9800" : "#4CAF50"
                                        visible: btEnabled
                                        Text {
                                            anchors.centerIn: parent
                                            text: btScanning ? "扫描中..." : "扫描"
                                            font.pixelSize: 22
                                            color: "#ffffff"
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                btScanning = true
                                                btStatusText = "正在扫描蓝牙设备..."
                                                btDeviceList.clear()
                                                btScanTimer.start()
                                            }
                                        }
                                    }
                                }

                                // 已连接提示
                                Text {
                                    visible: btConnectedDevice !== ""
                                    text: "已连接: " + btConnectedDevice
                                    font.pixelSize: 22
                                    color: "#2196F3"
                                    font.bold: true
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                }

                                // 状态提示
                                Text {
                                    visible: btStatusText !== ""
                                    text: btStatusText
                                    font.pixelSize: 22
                                    color: btScanning ? "#ff9800" : "#666666"
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                }

                                // 分隔线
                                Rectangle {
                                    width: parent.width - 20
                                    height: 1
                                    color: "#e0e0e0"
                                    visible: btDeviceList.count > 0
                                }

                                // 设备列表
                                ListView {
                                    id: btListView
                                    width: parent.width - 20
                                    height: parent.height - 260
                                    model: btDeviceList
                                    clip: true
                                    visible: btDeviceList.count > 0

                                    delegate: Rectangle {
                                        width: btListView.width
                                        height: 60
                                        color: index % 2 === 0 ? "#fafafa" : "#ffffff"
                                        radius: 8

                                        Row {
                                            anchors.fill: parent
                                            anchors.margins: 10
                                            spacing: 15

                                            Rectangle {
                                                width: 40; height: 40; radius: 20
                                                color: "#2196F3"
                                                Text {
                                                    anchors.centerIn: parent
                                                    text: "B"
                                                    font.pixelSize: 22
                                                    color: "#ffffff"
                                                    font.bold: true
                                                }
                                            }

                                            Column {
                                                anchors.verticalCenter: parent.verticalCenter
                                                spacing: 4
                                                Text {
                                                    text: name
                                                    font.pixelSize: 22
                                                    color: "#333333"
                                                    font.bold: true
                                                }
                                                Text {
                                                    text: addr
                                                    font.pixelSize: 16
                                                    color: "#999999"
                                                }
                                            }
                                        }

                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                selectedBtName = name
                                                selectedBtAddr = addr
                                                btConfirmPopup.visible = true
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // ========== 蓝牙连接确认弹窗 ==========
                    Rectangle {
                        id: btConfirmPopup
                        anchors.fill: parent
                        color: "#80000000"
                        visible: false
                        z: 25

                        Rectangle {
                            width: 480
                            height: 220
                            anchors.centerIn: parent
                            color: "#ffffff"
                            radius: 16
                            border.color: "#cccccc"
                            border.width: 1

                            Column {
                                anchors.fill: parent
                                anchors.margins: 24
                                spacing: 20

                                Text {
                                    text: "蓝牙连接"
                                    font.pixelSize: 28
                                    font.bold: true
                                    color: "#333333"
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }

                                Text {
                                    text: "是否连接 \"" + selectedBtName + "\" ?"
                                    font.pixelSize: 24
                                    color: "#666666"
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }

                                Text {
                                    text: selectedBtAddr
                                    font.pixelSize: 18
                                    color: "#999999"
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }

                                Row {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    spacing: 40
                                    Rectangle {
                                        width: 140; height: 44
                                        color: "#4CAF50"; radius: 8
                                        Text {
                                            anchors.centerIn: parent
                                            text: "是"
                                            font.pixelSize: 24
                                            color: "#ffffff"
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                btConfirmPopup.visible = false
                                                btStatusText = "正在配对 " + selectedBtName + "..."
                                                btConnectTimer.start()
                                            }
                                        }
                                    }
                                    Rectangle {
                                        width: 140; height: 44
                                        color: "#cccccc"; radius: 8
                                        Text {
                                            anchors.centerIn: parent
                                            text: "否"
                                            font.pixelSize: 24
                                            color: "#ffffff"
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                btConfirmPopup.visible = false
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // 蓝牙扫描定时器
                    Timer {
                        id: btScanTimer
                        interval: 1500
                        repeat: false
                        onTriggered: {
                            // 先检测蓝牙硬件
                            var checkResult = ebutton.bluetoothCheck()
                            if (checkResult !== "OK") {
                                btScanning = false
                                btStatusText = "蓝牙设备加载失败"
                                return
                            }
                            var result = ebutton.bluetoothScan()
                            btScanning = false
                            if (result === "") {
                                btStatusText = "未发现蓝牙设备"
                            } else {
                                var lines = result.split("\n")
                                btDeviceList.clear()
                                for (var i = 0; i < lines.length; i++) {
                                    if (lines[i] !== "") {
                                        var parts = lines[i].split(",")
                                        if (parts.length >= 2) {
                                            btDeviceList.append({"name": parts[1], "addr": parts[0]})
                                        }
                                    }
                                }
                                btStatusText = "发现 " + btDeviceList.count + " 个设备"
                            }
                        }
                    }

                    // 蓝牙连接定时器
                    Timer {
                        id: btConnectTimer
                        interval: 500
                        repeat: false
                        onTriggered: {
                            var conResult = ""
                            // 先检查是否已经配对
                            if (ebutton.bluetoothIsPaired(selectedBtAddr) === "OK") {
                                // 已配对，直接连接
                                conResult = ebutton.bluetoothConnect(selectedBtAddr)
                            } else {
                                // 未配对，先配对再连接
                                var pairResult = ebutton.bluetoothPair(selectedBtAddr)
                                if (pairResult === "OK") {
                                    conResult = ebutton.bluetoothConnect(selectedBtAddr)
                                } else {
                                    btStatusText = "配对失败"
                                    return
                                }
                            }
                            if (conResult === "OK") {
                                btConnectedDevice = selectedBtName
                                connectedBtAddr = selectedBtAddr
                                btStatusText = "连接成功"
                            } else {
                                btStatusText = "连接失败"
                            }
                        }
                    }

                    // ========== WiFi密码输入弹窗（键盘从底部弹出） ==========
                    Rectangle {
                        id: wifiPasswordPopup
                        anchors.fill: parent
                        color: "#80000000"
                        visible: false
                        z: 20

                        property bool shiftOn: false

                        MouseArea {
                            anchors.fill: parent
                            // 阻止点击穿透
                        }

                        // 密码输入卡片（居中靠上）
                        Rectangle {
                            id: passwordCard
                            width: 700
                            height: 300
                            anchors.centerIn: parent
                            anchors.verticalCenterOffset: -220
                            color: "#ffffff"
                            radius: 16
                            border.color: "#cccccc"
                            border.width: 1

                            Column {
                                anchors.fill: parent
                                anchors.margins: 28
                                spacing: 20

                                // SSID 标题
                                Text {
                                    text: "连接: " + selectedSSID
                                    font.pixelSize: 30
                                    font.bold: true
                                    color: "#333333"
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }

                                // 密码输入框
                                Rectangle {
                                    width: parent.width
                                    height: 56
                                    color: "#f5f5f5"
                                    radius: 10
                                    border.color: "#cccccc"
                                    border.width: 1
                                    Row {
                                        anchors.fill: parent
                                        anchors.margins: 10
                                        spacing: 8
                                        TextInput {
                                            id: passwordInput
                                            width: parent.width - 56
                                            height: parent.height
                                            font.pixelSize: 26
                                            echoMode: TextInput.Password
                                            focus: true
                                            verticalAlignment: TextInput.AlignVCenter
                                            Text {
                                                anchors.fill: parent
                                                font.pixelSize: 26
                                                color: "#aaaaaa"
                                                text: "输入密码"
                                                visible: passwordInput.text === ""
                                                verticalAlignment: Text.AlignVCenter
                                            }
                                        }
                                        Rectangle {
                                            width: 48; height: parent.height
                                            color: "#e0e0e0"; radius: 6
                                            Text {
                                                anchors.centerIn: parent
                                                text: passwordInput.echoMode === TextInput.Password ? "显示" : "隐藏"
                                                font.pixelSize: 16
                                                color: "#666666"
                                            }
                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: {
                                                    passwordInput.echoMode = passwordInput.echoMode === TextInput.Password ? TextInput.Normal : TextInput.Password
                                                }
                                            }
                                        }
                                    }
                                }

                                // 确定 / 取消
                                Row {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    spacing: 36
                                    Rectangle {
                                        width: 180; height: 50
                                        color: "#4CAF50"; radius: 10
                                        Text {
                                            anchors.centerIn: parent
                                            text: "确定"
                                            font.pixelSize: 26
                                            color: "#ffffff"
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                wifiPasswordPopup.visible = false
                                                wifiPasswordPopup.shiftOn = false
                                                wifiStatusText = "正在连接 " + selectedSSID + "..."
                                                wifiConnectTimer.start()
                                            }
                                        }
                                    }
                                    Rectangle {
                                        width: 180; height: 50
                                        color: "#cccccc"; radius: 10
                                        Text {
                                            anchors.centerIn: parent
                                            text: "取消"
                                            font.pixelSize: 26
                                            color: "#ffffff"
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                passwordInput.text = ""
                                                wifiPasswordPopup.shiftOn = false
                                                wifiPasswordPopup.visible = false
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // ===== 软键盘（从底部弹出） =====
                        Rectangle {
                            id: keyboardPanel
                            width: parent.width
                            height: 440
                            anchors.horizontalCenter: parent.horizontalCenter
                            y: wifiPasswordPopup.visible ? parent.height - height : parent.height
                            color: "#e8e8e8"
                            z: 10

                            Behavior on y {
                                NumberAnimation { duration: 250; easing.type: Easing.OutCubic }
                            }

                            Column {
                                anchors.centerIn: parent
                                spacing: 8
                                property int keyW: 96
                                property int keyH: 64

                                // 第1行: 数字
                                Row {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    spacing: 8
                                    Repeater {
                                        model: ["1","2","3","4","5","6","7","8","9","0","BkSp"]
                                        Rectangle {
                                            width: parent.parent.keyW; height: parent.parent.keyH
                                            color: index < 10 ? "#ffffff" : "#ffab91"
                                            radius: 8
                                            Text {
                                                anchors.centerIn: parent
                                                text: index < 10 ? modelData : "←"
                                                font.pixelSize: 28
                                                color: "#333333"
                                            }
                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: {
                                                    if (index < 10) {
                                                        passwordInput.text += modelData
                                                    } else {
                                                        passwordInput.text = passwordInput.text.substring(0, passwordInput.text.length - 1)
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                // 第2行: q-p
                                Row {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    spacing: 8
                                    Repeater {
                                        model: wifiPasswordPopup.shiftOn ?
                                            ["Q","W","E","R","T","Y","U","I","O","P"] :
                                            ["q","w","e","r","t","y","u","i","o","p"]
                                        Rectangle {
                                            width: parent.parent.keyW; height: parent.parent.keyH
                                            color: "#ffffff"
                                            radius: 8
                                            Text {
                                                anchors.centerIn: parent
                                                text: modelData
                                                font.pixelSize: 28
                                                color: "#333333"
                                            }
                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: { passwordInput.text += modelData }
                                            }
                                        }
                                    }
                                }

                                // 第3行: a-l
                                Row {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    spacing: 8
                                    Repeater {
                                        model: wifiPasswordPopup.shiftOn ?
                                            ["A","S","D","F","G","H","J","K","L"] :
                                            ["a","s","d","f","g","h","j","k","l"]
                                        Rectangle {
                                            width: index === 0 ? parent.parent.keyW + 55 : parent.parent.keyW
                                            height: parent.parent.keyH
                                            color: "#ffffff"
                                            radius: 8
                                            Text {
                                                anchors.centerIn: parent
                                                text: modelData
                                                font.pixelSize: 28
                                                color: "#333333"
                                            }
                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: { passwordInput.text += modelData }
                                            }
                                        }
                                    }
                                }

                                // 第4行: Shift + z-m + 特殊符号
                                Row {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    spacing: 8
                                    Repeater {
                                        model: {
                                            var letters = wifiPasswordPopup.shiftOn ?
                                                ["Z","X","C","V","B","N","M"] :
                                                ["z","x","c","v","b","n","m"]
                                            return ["Shift"].concat(letters).concat(["@","."])
                                        }
                                        Rectangle {
                                            width: index === 0 ? parent.parent.keyW + 20 : parent.parent.keyW
                                            height: parent.parent.keyH
                                            color: index === 0 ? (wifiPasswordPopup.shiftOn ? "#bbdefb" : "#ffffff") : "#ffffff"
                                            radius: 8
                                            Text {
                                                anchors.centerIn: parent
                                                text: modelData
                                                font.pixelSize: index === 0 ? 22 : 28
                                                color: "#333333"
                                            }
                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: {
                                                    if (index === 0) {
                                                        wifiPasswordPopup.shiftOn = !wifiPasswordPopup.shiftOn
                                                    } else {
                                                        passwordInput.text += modelData
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                // 第5行: 符号 + 空格
                                Row {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    spacing: 8
                                    Repeater {
                                        model: ["-","_","!","?","/","空格"]
                                        Rectangle {
                                            width: index === 5 ? parent.parent.keyW * 2 + 8 : parent.parent.keyW
                                            height: parent.parent.keyH
                                            color: index === 5 ? "#d5d5d5" : "#ffffff"
                                            radius: 8
                                            Text {
                                                anchors.centerIn: parent
                                                text: modelData
                                                font.pixelSize: 26
                                                color: "#555555"
                                            }
                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: {
                                                    if (index === 5) {
                                                        passwordInput.text += " "
                                                    } else {
                                                        passwordInput.text += modelData
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // WiFi扫描定时器
                    Timer {
                        id: wifiScanTimer
                        interval: 1500
                        repeat: false
                        onTriggered: {
                            var result = ebutton.wifiScan()
                            wifiSSIDList.clear()
                            if (result !== "") {
                                var lines = result.split("\n")
                                for (var i = 0; i < lines.length; i++) {
                                    var ssid = lines[i].trim()
                                    if (ssid !== "" && ssid !== "\\x00") {
                                        wifiSSIDList.append({modelData: ssid})
                                    }
                                }
                                if (wifiSSIDList.count === 0) {
                                    wifiStatusText = "未扫描到WiFi"
                                } else {
                                    wifiStatusText = "已扫描到 " + wifiSSIDList.count + " 个WiFi"
                                }
                            } else {
                                wifiStatusText = "未扫描到WiFi"
                            }
                            wifiScanning = false
                        }
                    }

                    // WiFi密码延迟连接定时器（等密码弹窗关闭）
                    Timer {
                        id: wifiConnectTimer
                        interval: 300
                        repeat: false
                        onTriggered: {
                            if (passwordInput.text === "") {
                                wifiStatusText = "密码不能为空"
                                return
                            }
                            var result = ebutton.wifiConnect(selectedSSID, passwordInput.text)
                            if (result.indexOf("wpa_state=COMPLETED") !== -1) {
                                connectedSSID = selectedSSID
                                wifiStatusText = ""
                            } else {
                                connectedSSID = ""
                                wifiStatusText = "连接失败: " + selectedSSID
                            }
                        }
                    }
                }

                // ========== 时间设置子页面 ==========
                Rectangle {
                    id: shijian_page
                    anchors.fill: parent
                    color: "#e8e8e8"
                    visible: false
                    z: 10

                    Rectangle {
                        width: parent.width
                        height: 140
                        color: "#ffffff"

                        Image {
                            anchors.left: parent.left
                            anchors.leftMargin: 30
                            anchors.verticalCenter: parent.verticalCenter
                            source: "photo/fanhui.png"
                            width: 80
                            height: 80
                            MouseArea {
                                anchors.fill: parent
                                onClicked: shijian_page.visible = false
                            }
                        }

                        Text {
                            id:timeTitle
                            anchors.centerIn: parent
                            color: "#000000"
                            text: "时间设置"
                            font.bold: true
                            font.pixelSize: 48
                        }
                    }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.leftMargin: 40
                        anchors.right: parent.right
                        anchors.rightMargin: 40
                        anchors.top: parent.top
                        anchors.topMargin: 160
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 20
                        color: "#ffffff"
                        radius: 30

                        Column {
                            anchors.centerIn: parent
                            spacing: 60

                            // 格式提示
                            Text {
                                visible: false
                                text: "YYYY - MM - DD     HH : mm : ss"
                                font.pixelSize: 30
                                color: "#999999"
                                anchors.horizontalCenter: parent.horizontalCenter
                            }

                            // 时间选择区域
                            Row {
                                spacing: 0
                                anchors.horizontalCenter: parent.horizontalCenter

                                // 通用时间字段组件
                                Component {
                                    id: timeFieldComp

                                    Column {
                                        spacing: 8
                                        anchors.verticalCenter: parent.verticalCenter

                                        // + 按钮
                                        Rectangle {
                                            width: 80
                                            height: 80
                                            radius: 40
                                            color: maUp.pressed ? "#e0e0e0" : "#f5f5f5"
                                            border.color: "#dddddd"
                                            border.width: 2
                                            anchors.horizontalCenter: parent.horizontalCenter

                                            Text {
                                                text: "▲"
                                                font.pixelSize: 28
                                                color: "#666666"
                                                anchors.centerIn: parent
                                            }

                                            MouseArea {
                                                id: maUp
                                                anchors.fill: parent
                                                onClicked: control.onIncrement()
                                            }
                                        }

                                        // 数字显示
                                        Rectangle {
                                            width: 160
                                            height: 120
                                            radius: 16
                                            color: "#fafafa"
                                            border.color: "#e0e0e0"
                                            border.width: 2

                                            Text {
                                                anchors.centerIn: parent
                                                text: control.value
                                                font.pixelSize: 64
                                                font.bold: true
                                                color: "#333333"
                                            }
                                        }

                                        // - 按钮
                                        Rectangle {
                                            width: 80
                                            height: 80
                                            radius: 40
                                            color: maDown.pressed ? "#e0e0e0" : "#f5f5f5"
                                            border.color: "#dddddd"
                                            border.width: 2
                                            anchors.horizontalCenter: parent.horizontalCenter

                                            Text {
                                                text: "▼"
                                                font.pixelSize: 28
                                                color: "#666666"
                                                anchors.centerIn: parent
                                            }

                                            MouseArea {
                                                id: maDown
                                                anchors.fill: parent
                                                onClicked: control.onDecrement()
                                            }
                                        }

                                        // 标签
                                        Text {
                                            text: control.label
                                            font.pixelSize: 32
                                            color: "#999999"
                                            anchors.horizontalCenter: parent.horizontalCenter
                                        }
                                    }
                                }

                                // 年
                                Loader {
                                    sourceComponent: timeFieldComp
                                    property var control: ({
                                        value: year,
                                        //label: "年",
                                        onIncrement: function() { year += 1 },
                                        onDecrement: function() { year -= 1 }
                                    })
                                }

                                // 分隔符
                                Text { text: "  "; font.pixelSize: 60; anchors.verticalCenter: parent.verticalCenter }

                                // 月
                                Loader {
                                    sourceComponent: timeFieldComp
                                    property var control: ({
                                        value: ("0" + yue).slice(-2),
                                        //label: "月",
                                        onIncrement: function() { if (yue < 12) yue += 1 },
                                        onDecrement: function() { if (yue > 1) yue -= 1 }
                                    })
                                }

                                // 分隔符
                                Text { text: "  "; font.pixelSize: 60; anchors.verticalCenter: parent.verticalCenter }

                                // 日
                                Loader {
                                    sourceComponent: timeFieldComp
                                    property var control: ({
                                        value: ("0" + ri).slice(-2),
                                        //label: "日",
                                        onIncrement: function() { if (ri < 31) ri += 1 },
                                        onDecrement: function() { if (ri > 1) ri -= 1 }
                                    })
                                }

                                // 空格分隔
                                Text { text: "      "; font.pixelSize: 60 }

                                // 时
                                Loader {
                                    sourceComponent: timeFieldComp
                                    property var control: ({
                                        value: ("0" + s).slice(-2),
                                        //label: "时",
                                        onIncrement: function() { if (s < 23) s += 1 },
                                        onDecrement: function() { if (s > 0) s -= 1 }
                                    })
                                }

                                // 冒号
                                Text {
                                    text: ":"
                                    font.pixelSize: 64
                                    font.bold: true
                                    color: "#666666"
                                    anchors.verticalCenter: parent.verticalCenter
                                }

                                // 分
                                Loader {
                                    sourceComponent: timeFieldComp
                                    property var control: ({
                                        value: ("0" + f).slice(-2),
                                        //label: "分",
                                        onIncrement: function() { if (f < 59) f += 1 },
                                        onDecrement: function() { if (f > 0) f -= 1 }
                                    })
                                }

                                // 冒号
                                Text {
                                    text: ":"
                                    font.pixelSize: 64
                                    font.bold: true
                                    color: "#666666"
                                    anchors.verticalCenter: parent.verticalCenter
                                }

                                // 秒
                                Loader {
                                    sourceComponent: timeFieldComp
                                    property var control: ({
                                        value: ("0" + m).slice(-2),
                                        //label: "秒",
                                        onIncrement: function() { if (m < 59) m += 1 },
                                        onDecrement: function() { if (m > 0) m -= 1 }
                                    })
                                }
                            }

                            // 按钮行
                            Row {
                                spacing: 40
                                anchors.horizontalCenter: parent.horizontalCenter

                                Rectangle {
                                    width: 260
                                    height: 80
                                    radius: 40
                                    color: btnOk.pressed ? "#1565C0" : "#1976D2"

                                    Text {
                                        id:timeConfirm
                                        anchors.centerIn: parent
                                        text: "确定"
                                        font.pixelSize: 36
                                        color: "#ffffff"
                                    }

                                    MouseArea {
                                        id: btnOk
                                        anchors.fill: parent
                                        onClicked: {
                                            times.stop()
                                            ebutton.settime(year, yue, ri, s, f, m)
                                            shijian = ebutton.gettime()
                                            times.start()
                                            testtr1.visible = true
                                            shijian_page.visible = false
                                        }
                                    }
                                }

                                Rectangle {
                                    width: 260
                                    height: 80
                                    radius: 40
                                    color: btnCancel.pressed ? "#e0e0e0" : "#f5f5f5"
                                    border.color: "#cccccc"
                                    border.width: 2

                                    Text {
                                        id:timeCancel
                                        anchors.centerIn: parent
                                        text: "取消"
                                        font.pixelSize: 36
                                        color: "#666666"
                                    }

                                    MouseArea {
                                        id: btnCancel
                                        anchors.fill: parent
                                        onClicked: shijian_page.visible = false
                                    }
                                }
                            }
                        }
                    }
                }
    }

    ApplicationWindow {
        id:jifen_jiemian
        Component.onCompleted: {
            jifen_jiemian.raise()  // 将窗口提升到最前面
        }
        visible: false
        x:700
        y:250
        width: 500
        height: 500
        // 设置无边框窗口
        flags: Qt.FramelessWindowHint
        // 设置透明背景
        color: "transparent"
        // 初始化语言状态属性
        property bool isChinese: true
        // 主要布局
        Rectangle {
            width: 500
            height: 500
            color: "grey" // 设置背景色为黑色
            // ListView
            ListView {
                id: listView_jifen
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 10 // 防止内容靠得太近

                model: ListModel {
                    ListElement { name: "OFF" }
                    ListElement { name: "X2" }
                    ListElement { name: "X4" }
                    ListElement { name: "X8" }
                    ListElement { name: "X16" }
                }

                // 处理 currentIndex
                property int currentIndex: 0 // 初始化 currentIndex 为 -1

                delegate: Item {
                    width: 500
                    height: 250

                    // 左侧的内容区域
                    Rectangle {
                        width: 300
                        height: 250
                        color: "grey" // 黑色背景

                        Row {
                            anchors.centerIn: parent
                            spacing: 10

                            // 自定义文本显示
                            RadioButton {
                                //id: radioButton
                                // 使用 scale 放大整个控件
                                scale: 3  // 放大 1.5 倍
                                checked: listView_jifen.currentIndex === index // 确保只有一个选中
                                onClicked: {
                                    // 设置当前选中的项
                                    listView_jifen.currentIndex = index;
                                    console.log(index + " 被选中");
                                    ebutton.setjf(index);
                                    jifen_jiemian.visible = false;

                                }

                                // 添加自定义文本，调整其颜色
                                Text {
                                    x:60
                                    text: model.name
                                    color: "white" // 设置文字颜色为白色
                                    verticalAlignment: Text.AlignVCenter
                                    anchors.verticalCenter: parent.verticalCenter
                                    // anchors.left: radioButton.right // 将文本放到 radioButton 的右侧

                                    anchors.margins: 10 // 添加一些间距
                                    font.pixelSize: 20  // 设置字号为24像素
                                }
                            }
                        }
                    }

                    // 分隔线
                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "white" // 分隔线颜色
                        anchors.bottom: parent.bottom
                    }
                }
            }
        }
    }



    ApplicationWindow {
        id:jishu_jiemian    // 确保窗口显示在最前面
        objectName: "2"
        Component.onCompleted: {
            jishu_jiemian.raise()  // 将窗口提升到最前面
        }
        visible: false

        x:700
        y:250
        width: 500
        height: 500
        // 设置无边框窗口
        flags: Qt.FramelessWindowHint
        // 设置透明背景
        color: "transparent"
        // 初始化语言状态属性
        property bool isChinese: true
        // 主要布局
        Rectangle {
            width: 500
            height: 500
            color: "grey" // 设置背景色为黑色
            // ListView
            ListView {
                id: listView_jishu
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 10 // 防止内容靠得太近

                model: ListModel {
                    ListElement { name: "/M" }
                    ListElement { name: "/S" }
                }

                // 处理 currentIndex
                property int currentIndex: 0 // 初始化 currentIndex 为 -1

                delegate: Item {
                    width: 500
                    height: 250

                    // 左侧的内容区域
                    Rectangle {
                        width: 300
                        height: 250
                        color: "grey" // 黑色背景

                        Row {
                            anchors.centerIn: parent
                            spacing: 10

                            // 自定义文本显示
                            RadioButton {
                                //id: radioButton
                                // 使用 scale 放大整个控件
                                scale: 3  // 放大 1.5 倍
                                checked: listView_jishu.currentIndex === index // 确保只有一个选中
                                onClicked: {
                                    // 设置当前选中的项
                                    listView_jishu.currentIndex = index;
                                    console.log(index + " 被选中");
                                    ebutton.xxxxxjishu(index);
                                    jishu_jiemian.visible = false;

                                }

                                // 添加自定义文本，调整其颜色
                                Text {
                                    x:60
                                    text: model.name
                                    color: "white" // 设置文字颜色为白色
                                    verticalAlignment: Text.AlignVCenter
                                    anchors.verticalCenter: parent.verticalCenter
                                    // anchors.left: radioButton.right // 将文本放到 radioButton 的右侧

                                    anchors.margins: 10 // 添加一些间距
                                    font.pixelSize: 20  // 设置字号为24像素
                                }
                            }
                        }
                    }

                    // 分隔线
                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "white" // 分隔线颜色
                        anchors.bottom: parent.bottom
                    }
                }
            }
        }
    }

    // ========== 摄像头画面左侧任务列表面板 ==========
    Rectangle {
        id: taskPanel
        visible: false
        x: 0
        y: 100
        width: 380
        height: taskPanel.collapsed ? 70 : parent.height - 350
        color: "transparent"
        z: 50
        property bool collapsed: false

        Behavior on height { NumberAnimation { duration: 200 } }

        // 面板标题
        Rectangle {
            width: parent.width
            height: 60
            color: "#ffffff"

            Text {
                anchors.centerIn: parent
                text: "任务列表"
                font.pixelSize: 32
                font.bold: true
                color: "#000000"
            }
        }

        // 收起/展开按钮
        Rectangle {
            id: stopTaskBtn
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.top: parent.top
            anchors.topMargin: 8
            width: 100
            height: 44
            radius: 8
            color: stopTaskMouse.pressed
                    ? (taskPanel.collapsed ? "#74c69d" : "#ff9a8e")
                    : (taskPanel.collapsed ? "#95d5b2" : "#ff8380")

            Behavior on color { ColorAnimation { duration: 150 } }

            Text {
                anchors.centerIn: parent
                text: taskPanel.collapsed ? "展开" : "收起"
                font.pixelSize: 24
                color: "#ffffff"
            }

            MouseArea {
                id: stopTaskMouse
                anchors.fill: parent
                onClicked: {
                    taskPanel.collapsed = !taskPanel.collapsed
                }
            }
        }

        // 任务内容列表
        ListView {
            id: taskPanelList
            visible: !taskPanel.collapsed
            anchors.top: parent.top
            anchors.topMargin: 70
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            clip: true
            spacing: 2
            model: contentModel
            property int selectedIndex: -1
            property int photographedIndex: -1

            delegate: Rectangle {
                width: ListView.view.width
                height: 56
                color: taskItemMouse.pressed ? "#d0d0d0"
                         : (taskPanelList.selectedIndex === index ? "#c8e6c9" : "transparent")

                //解决点击变黑
                //Behavior on color { ColorAnimation { duration: 120 } }

                Text {
                    anchors.fill: parent
                    text: model.lineText
                    font.pixelSize: 28
                    color: model.photographed ? "#bababa" : "#000000"
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 15
                    elide: Text.ElideRight
                }

                MouseArea {
                    id: taskItemMouse
                    anchors.fill: parent
                    onClicked: {
                        taskPanelList.selectedIndex = index
                        taskPanelList.photographedIndex = index
                        testtrch.text = model.lineText
                    }
                }
            }

            // 空列表提示
            Text {
                anchors.centerIn: parent
                text: "暂无任务数据"
                font.pixelSize: 28
                color: "#FFFFFF"
                visible: contentModel.count === 0
            }
        }
    }

    ApplicationWindow {
        id:ceju_jiemian    // 确保窗口显示在最前面
        objectName: "3"
        Component.onCompleted: {
            ceju_jiemian.raise()  // 将窗口提升到最前面
        }
        visible: false

        x:700
        y:250
        width: 500
        height: 500
        // 设置无边框窗口
        flags: Qt.FramelessWindowHint
        // 设置透明背景
        color: "transparent"
        // 初始化语言状态属性
        property bool isChinese: true
        // 主要布局
        Rectangle {
            width: 500
            height: 500
            color: "grey" // 设置背景色为黑色
            // ListView
            ListView {
                id: listView_ceju
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 10 // 防止内容靠得太近

                model: ListModel {
                    ListElement { name: "ON" }
                    ListElement { name: "OFF" }
                }

                // 处理 currentIndex
                property int currentIndex: 1 // 初始化 currentIndex 为 -1

                delegate: Item {
                    width: 500
                    height: 250

                    // 左侧的内容区域
                    Rectangle {
                        width: 300
                        height: 250
                        color: "grey" // 黑色背景

                        Row {
                            anchors.centerIn: parent
                            spacing: 10

                            // 自定义文本显示
                            RadioButton {
                                //id: radioButton
                                // 使用 scale 放大整个控件
                                scale: 3  // 放大 1.5 倍
                                checked: listView_ceju.currentIndex === index // 确保只有一个选中
                                onClicked: {
                                    // 设置当前选中的项
                                    listView_ceju.currentIndex = index;
                                    console.log(index + " 被选中");

                                    ceju_jiemian.visible = false;
                                    ebutton.settjiguang(index);

                                }

                                // 添加自定义文本，调整其颜色
                                Text {
                                    x:60
                                    text: model.name
                                    color: "white" // 设置文字颜色为白色
                                    verticalAlignment: Text.AlignVCenter
                                    anchors.verticalCenter: parent.verticalCenter
                                    // anchors.left: radioButton.right // 将文本放到 radioButton 的右侧

                                    anchors.margins: 10 // 添加一些间距
                                    font.pixelSize: 20  // 设置字号为24像素
                                }
                            }
                        }
                    }

                    // 分隔线
                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "white" // 分隔线颜色
                        anchors.bottom: parent.bottom
                    }
                }
            }
        }
    }


    ApplicationWindow {

        id:yuyan_jiemian    // 确保窗口显示在最前面
        objectName: "4"
        Component.onCompleted: {
            yuyan_jiemian.raise()  // 将窗口提升到最前面
        }
        visible: false

        x:700
        y:250
        width: 650
        height: 700
        // 设置无边框窗口
        flags: Qt.FramelessWindowHint
        // 设置透明背景
        color: "transparent"
        // 初始化语言状态属性
        property bool isChinese: true
        // 主要布局
        Rectangle {
            width: 1200
            height: 800
            color: "grey" // 设置背景色为黑色
            // ListView
            ListView {
                id: listView_yuyan
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 10 // 防止内容靠得太近

                model: ListModel {
                    ListElement { name: "中文" }
                    ListElement { name: "English" }
                    ListElement { name: "Русский" }
                    ListElement { name: "日本語" }
                    ListElement { name: "Italiano" }

                }

                // 处理 currentIndex
                property int currentIndex: 0 // 初始化 currentIndex 为 -1

                delegate: Item {
                    width: 700
                    height: 250

                    // 左侧的内容区域
                    Rectangle {
                        width: 300
                        height: 250
                        color: "grey" // 黑色背景

                        Row {
                            anchors.centerIn: parent
                            spacing: 10

                            // 自定义文本显示
                            RadioButton {
                                //id: radioButton
                                // 使用 scale 放大整个控件
                                scale: 3  // 放大 1.5 倍
                                checked: listView_yuyan.currentIndex === index // 确保只有一个选中
                                onClicked: {
                                    // 设置当前选中的项
                                    listView_yuyan.currentIndex = index;
                                    console.log(index + " 被选中");
                                    yuyan_jiemian.visible = false;
                                    if(index == 0)
                                    {
                                        listView.model.setProperty(0, "name", "红");
                                        listView.model.setProperty(1, "name", "白");
                                        listView.model.setProperty(2, "name", "蓝");
                                        listView.model.setProperty(3, "name", "黄");
                                        listView.model.setProperty(4, "name", "青");
                                        listView.model.setProperty(5, "name", "品红");
                                        listView.model.setProperty(6, "name", "玫红");
                                        listView.model.setProperty(7, "name", "紫");
                                        listView.model.setProperty(8, "name", "灰");
                                        listView.model.setProperty(9, "name", "深绿");
                                        listView.model.setProperty(10, "name", "淡蓝");
                                        listView.model.setProperty(11, "name", "绿");
                                        listView1.model.setProperty(0, "name", "积分");
                                        listView1.model.setProperty(1, "name", "计数单位");
                                        listView1.model.setProperty(2, "name", "阈值");
                                        listView1.model.setProperty(3, "name", "台账功能");
                                        listView1.model.setProperty(4, "name", "测距开关");
                                        listView1.model.setProperty(5, "name", "wifi/蓝牙");
                                        listView1.model.setProperty(6, "name", "语言");
                                        listView1.model.setProperty(7, "name", "时间设置");
                                        listView1.model.setProperty(8, "name", "定位开关");
                                        factorCorrectionTitle.text = "因子矫正";
                                        shezhi_biaoti_1 = "设置";
                                        yanseshezhi_biaoti_1 = "颜色设置";
                                        testtr.text = "紫红光电 UVIRSYS";
                                        //testtr.text = "武高电测";
                                        //quezhi_1.text = "阈值";
                                        thresholdInputDialog.title = "设置阈值";
                                        thresholdInputDialog_1.text = "输入阈值（0-10000）";
                                        queding_2.text = "确定";
                                        quxiao_2.text = "取消";
                                        taizhang.title = "导入台账";
                                        datetimeInputDialog.title = "输入日期和时间";
                                        time_ok.text = "确定";
                                        time_off.text = "取消";
                                        dialog.title = "文件夹";
                                        fullScreenDialog.title = "全屏图像";
                                        testtr.font.pixelSize = 65;
                                        ebutton.setzywen(0);
                                        ebutton.setyuyan(0);
                                        dynamicText = ebutton.getzy();
                                        ebutton.switch_l(1);
                                        updateModeText()

                                    }else if(index == 1) {
                                        listView.model.setProperty(0, "name", "Red");
                                        listView.model.setProperty(1, "name", "White");
                                        listView.model.setProperty(2, "name", "Blue");
                                        listView.model.setProperty(3, "name", "Yellow");
                                        listView.model.setProperty(4, "name", "Cyan");
                                        listView.model.setProperty(5, "name", "Magenta");
                                        listView.model.setProperty(6, "name", "Pink");
                                        listView.model.setProperty(7, "name", "Purple");
                                        listView.model.setProperty(8, "name", "Gray");
                                        listView.model.setProperty(9, "name", "Dark Green");
                                        listView.model.setProperty(10, "name", "Light Blue");
                                        listView.model.setProperty(11, "name", "Green");
                                        listView1.model.setProperty(0, "name", "Points");
                                        listView1.model.setProperty(1, "name", "Counting Unit");
                                        listView1.model.setProperty(2, "name", "Threshold");
                                        listView1.model.setProperty(3, "name", "Ledger Function");
                                        listView1.model.setProperty(4, "name", "Distance Switch");
                                        listView1.model.setProperty(5, "name", "Wi-Fi/Bluetooth");
                                        listView1.model.setProperty(6, "name", "Language");
                                        listView1.model.setProperty(7, "name", "Time Settings");
                                        listView1.model.setProperty(8, "name", "Positioning Switch");
                                        factorCorrectionTitle.text = "Factor Correction";
                                        shezhi_biaoti_1 = "set up";
                                        yanseshezhi_biaoti_1 = "color set";
                                        testtr.text = "UVIRSYS";
                                        //quezhi_1.text = "threshold";
                                        thresholdInputDialog.title = "Set threshold";
                                        thresholdInputDialog_1.text = "Input threshold(0-10000)";
                                        queding_2.text = "sure";
                                        quxiao_2.text = "cancel";
                                        taizhang.title = "Import ledger";
                                        datetimeInputDialog.title = "Enter date and time";
                                        time_ok.text = "sure";
                                        time_off.text = "cancel";
                                        dialog.title = "Folder";
                                        fullScreenDialog.title = "Full-screen image";
                                        testtr.font.pixelSize = 60;
                                        ebutton.setyuyan(1);
                                        ebutton.setzywen(1);
                                        dynamicText = ebutton.getzy();
                                        ebutton.switch_l(0);

                                        updateModeText()


                                    }else if(index == 3) {
                                        listView.model.setProperty(0, "name", "レッド");
                                        listView.model.setProperty(1, "name", "ホワイト");
                                        listView.model.setProperty(2, "name", "ブルー");
                                        listView.model.setProperty(3, "name", "イエロー");
                                        listView.model.setProperty(4, "name", "スカイブルー");
                                        listView.model.setProperty(5, "name", "マゼンタ");
                                        listView.model.setProperty(6, "name", "ローズレッド");
                                        listView.model.setProperty(7, "name", "パープル");
                                        listView.model.setProperty(8, "name", "灰（はい）");
                                        listView.model.setProperty(9, "name", "深緑");
                                        listView.model.setProperty(10, "name", "ディープブルー");
                                        listView.model.setProperty(11, "name", "グリーン");
                                        listView1.model.setProperty(0, "name", "積分");
                                        listView1.model.setProperty(1, "name", "カウント単位");
                                        listView1.model.setProperty(2, "name", "しきい値");
                                        listView1.model.setProperty(3, "name", "台帳機能");
                                        listView1.model.setProperty(4, "name", "レンジスイッチ");
                                        listView1.model.setProperty(5, "name", "wifi/Blue");
                                        listView1.model.setProperty(6, "name", "言語");
                                        listView1.model.setProperty(7, "name", "時刻設定");
                                        listView1.model.setProperty(8, "name", "位置スイッチ");
                                        factorCorrectionTitle.text = "係数補正";
                                        shezhi_biaoti_1 = "設定";
                                        yanseshezhi_biaoti_1 = "色設定";
                                        testtr.text = "UVIRSYS";
                                        //testtr.text = "武高电测";
                                        //quezhi_1.text = "阈值";
                                        thresholdInputDialog.title = "しきい値の設定";
                                        thresholdInputDialog_1.text = "しきい値の入力（0～10000）";
                                        queding_2.text = "確定";
                                        quxiao_2.text = "キャンセル";
                                        taizhang.title = "導入台帳";
                                        datetimeInputDialog.title = "日付と時刻の入力";
                                        time_ok.text = "確定";
                                        time_off.text = "キャンセル";
                                        dialog.title = "フォルダ";
                                        fullScreenDialog.title = "全画面画像";
                                        testtr.font.pixelSize = 65;
                                        ebutton.setzywen(1);
                                        ebutton.setyuyan(4);
                                        dynamicText = ebutton.getzy();
                                        ebutton.switch_l(0);

                                        updateModeText()


                                    }else if(index == 4){
                                        ydl_l.running = true;
                                    }
                                    else {
                                        listView.model.setProperty(0, "name", "Красный");
                                        listView.model.setProperty(1, "name", "Белый");
                                        listView.model.setProperty(2, "name", "Синий");
                                        listView.model.setProperty(3, "name", "Желтый");
                                        listView.model.setProperty(4, "name", "Циан");
                                        listView.model.setProperty(5, "name", "Магента");
                                        listView.model.setProperty(6, "name", "Розовый");
                                        listView.model.setProperty(7, "name", "Фиолетовый");
                                        listView.model.setProperty(8, "name", "Серый");
                                        listView.model.setProperty(9, "name", "Темно-зеленый");
                                        listView.model.setProperty(10, "name", "Светло-голубой");
                                        listView.model.setProperty(11, "name", "Зеленый");
                                        listView1.model.setProperty(0, "name", "Очки");
                                        listView1.model.setProperty(1, "name", "Единица измерения");
                                        listView1.model.setProperty(2, "name", "Порог");
                                        listView1.model.setProperty(3, "name", "Функция бухгалтерского учета");
                                        listView1.model.setProperty(4, "name", "Переключатель расстояния");
                                        listView1.model.setProperty(5, "name", "Wi-Fi/Bluetooth");
                                        listView1.model.setProperty(6, "name", "Язык");
                                        listView1.model.setProperty(7, "name", "Настройки времени");
                                        listView1.model.setProperty(8, "name", "позиционный переключатель");
                factorCorrectionTitle.text = "Коррекция факторов";
                                        shezhi_biaoti_1 = "настройка";
                                        yanseshezhi_biaoti_1 = "установка цвета";
                                        testtr.text = "UVIRSYS";
                                        // quezhi_1.text = "порог";
                                        thresholdInputDialog.title = "Установить порог";
                                        thresholdInputDialog_1.text = "Введите порог (0-10000)";
                                        queding_2.text = "подтвердить";
                                        quxiao_2.text = "отменить";
                                        taizhang.title = "Импорт бухгалтерской книги";
                                        datetimeInputDialog.title = "Введите дату и время";
                                        time_ok.text = "подтвердить";
                                        time_off.text = "отменить";
                                        dialog.title = "папка";
                                        fullScreenDialog.title = "Полноэкранное изображение";
                                        testtr.font.pixelSize = 60;
                                        ebutton.setyuyan(2);
                                        ebutton.setzywen(1);
                                        dynamicText = ebutton.getzy();
                                        ebutton.switch_l(0);

                                        updateModeText()

                                    }
                                }

                                // 添加自定义文本，调整其颜色
                                Text {
                                    x:60
                                    text: model.name
                                    color: "white" // 设置文字颜色为白色
                                    verticalAlignment: Text.AlignVCenter
                                    anchors.verticalCenter: parent.verticalCenter
                                    // anchors.left: radioButton.right // 将文本放到 radioButton 的右侧

                                    anchors.margins: 10 // 添加一些间距
                                    font.pixelSize: 20  // 设置字号为24像素
                                }
                            }
                        }
                    }

                    // 分隔线
                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "white" // 分隔线颜色
                        anchors.bottom: parent.bottom
                    }
                }
            }
        }
    }


    // 分隔线0
    Rectangle {
        x: 1700
        y: 101
        width: 200
        height: 1
        color: "#3a3a3a"
        visible: ebutton.mssz === 3
        z: 1001
    }

    //draw
// 伪彩调整按钮 - 放在最上面
Button {
    id: pseudoColorButton
    objectName: "pseudoColorButton"
    x: 1700
    y: 100
    width: 200
    height: 160
    visible: ebutton.mssz === 3  // 红外模式下常显
    z: 1000
    hoverEnabled: true

    onClicked: {
        console.log("伪彩按钮被点击！")
        showPseudoColorMenu = !showPseudoColorMenu
        showIrOptions = false
        showDrawLineMenu = false
        showDrawPointMenu = false
    }

    background: Rectangle {
        color: pseudoColorButton.down ? "white" : "##1c1c1c"
        radius: 4
        border.width: 1
        border.color: "transparent"  // 完全不要边框
    }

    contentItem: Text {
        text: {
            var lang = ebutton.yuyan
            if (lang === 0) return "伪彩"
            else if(lang === 4) return "偽彩"
            else if (lang === 1) return "Pseudo"
            else if (lang === 5) return "Pseudocolore"
            else return "Псевдоколор"
        }
        font.family: "Microsoft YaHei"  // 字体名称
        color: "white"
        font.bold: false
        font.pixelSize: 50
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}

    // 分隔线1 - 伪彩按钮和画框按钮之间
    Rectangle {
        x: 1700
        y: 260
        width: 200
        height: 1
        color: "#3a3a3a"
        visible: ebutton.mssz === 3
        z: 1001
    }

    // 画框按钮 - 点击后不消失
    Button {
        id: irOptionsButton
        objectName: "irOptionsButton"
        x: 1700
        y: 261
        width: 200
        height: 160
        visible: ebutton.mssz === 3  // 红外模式下常显，点击后不消失
        z: 1000  // 最高层级
        hoverEnabled: true

        onClicked: {
            console.log("画框按钮被点击！")
            showIrOptions = !showIrOptions  // 切换显示/隐藏
            showPseudoColorMenu = false
            showDrawLineMenu = false
            showDrawPointMenu = false
            console.log("showIrOptions =", showIrOptions)
        }

        background: Rectangle {
            color: irOptionsButton.down ? "#4a6a8a" : "##1c1c1c"
            radius: 4
            border.width: 1
            border.color: "transparent"  // 完全不要边框
        }

        contentItem: Text {
            text: {
                var lang = ebutton.yuyan
                if (lang === 0) return "框"
                else if(lang === 4) return "フレーム"
                else if (lang === 1) return "Frame"
                else if (lang === 5) return "Cornice"
                else return "Рамка"
            }
            font.family: "Microsoft YaHei"  // 字体名称
            color: "white"
            font.bold: false
            font.pixelSize: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    // 分隔线2 - 画框按钮和画线按钮之间
    Rectangle {
        x: 1700
        y: 421
        width: 200
        height: 1
        color: "#3a3a3a"
        visible: ebutton.mssz === 3
        z: 1001
    }

    // 画线按钮 - 点击后不消失
    Button {
        id: drawLineButton
        objectName: "drawLineButton"
        x: 1700
        y: 422
        width: 200
        height: 160
        visible: ebutton.mssz === 3  // 红外模式下常显，点击后不消失
        z: 1000
        hoverEnabled: true


        onClicked: {
            console.log("画线按钮被点击！")
            showDrawLineMenu = !showDrawLineMenu  // 切换显示/隐藏
            showPseudoColorMenu = false
            showIrOptions = false
            showDrawPointMenu = false
        }

        background: Rectangle {
            color: drawLineButton.down ? "#4a6a8a" : "##1c1c1c"
            radius: 4
            border.width: 1
            border.color: "transparent"  // 完全不要边框
        }

        contentItem: Text {

            text: {
                var lang = ebutton.yuyan
                if (lang === 0) return "线";      // 中文简体
                if (lang === 4) return "線";      // 中文繁体
                if (lang === 1) return "Line";    // 英语
                if (lang === 5) return "Linea";   // 西班牙语
                return "Линия";                   // 俄语
            }
            font.family: "Microsoft YaHei"  // 字体名称
            color: "white"
            font.bold: false
            font.pixelSize: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    // 分隔线3 - 画线按钮和画点按钮之间
    Rectangle {
        x: 1700
        y: 582
        width: 200
        height: 1
        color: "#3a3a3a"
        visible: ebutton.mssz === 3
        z: 1001
    }

    // 画线菜单弹窗 - 向左展开
    Rectangle {
        id: drawLinePopup
        x: 1500
        y: 400
        width: 200
        height: 230
        radius: 10
        visible: showDrawLineMenu && ebutton.mssz === 3
        z: 1001
        border.width: 2
        border.color: "#333"
        color: "white"

        Column {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            Button {

                width: parent.width - 20
                height: 70
                font.pixelSize: 26

                background: Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                    border.width: 1
                    border.color: "#ccc"
                }
                contentItem: Text {
                    text: {
                        var lang = ebutton.yuyan
                        if (lang === 0) return "放置"
                        else if (lang === 4) return "放置"
                        else if (lang === 1) return "Place"
                        else if (lang === 5) return "Posizione"
                        else return "Положить"
                    }
                    color: "#333"
                    font.bold: true
                    font.pixelSize: 26
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                // 画线菜单的放置按钮
                onClicked: {
                    showIrLineTool = true
                    //showIrRectangle = false

                    // 设置水平线坐标
                    irLinePt1X = 500
                    irLinePt1Y = 540
                    irLinePt2X = 1420
                    irLinePt2Y = 540

                    // 手动更新中点坐标 - 添加这两行
                    irLineMidPtX = (irLinePt1X + irLinePt2X) / 2
                    irLineMidPtY = (irLinePt1Y + irLinePt2Y) / 2

                    showDrawLineMenu = false
                    console.log("直线已放置，中点：", irLineMidPtX, irLineMidPtY)
                }
            }

            Button {
                width: parent.width - 20
                height: 70
                font.pixelSize: 26

                background: Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                    border.width: 1
                    border.color: "#ccc"
                }
                contentItem: Text {
                    text: {
                        var lang = ebutton.yuyan
                        if (lang === 0) return "清除"
                        else if (lang === 4) return "クリア"
                        else if (lang === 1) return "Clear"
                        else if (lang === 5) return "Cancella"
                        else return "Очистить"
                    }
                    color: "#333"
                    font.bold: true
                    font.pixelSize: 26
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    showIrLineTool = false
                    showDrawLineMenu = false
                }
            }

            Button {
                text: "关闭"
                width: parent.width - 20
                height: 50
                font.pixelSize: 22

                background: Rectangle {
                    color: "#e0e0e0"
                    radius: 8
                }
                contentItem: Text {
                    text: {
                        var lang = ebutton.yuyan
                        if (lang === 0) return "关闭"
                        else if (lang === 1) return "Close"
                        else if (lang === 4) return "閉じる"
                        else if (lang === 5) return "Chiudi"
                        else return "Закрыть"
                    }
                    color: "#666"
                    font.pixelSize: 22
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    showDrawLineMenu = false
                }
            }
        }
    }

    // 画点按钮 - 点击后不消失
    Button {
        id: drawPointButton
        objectName: "drawPointButton"
        x: 1700
        y: 583
        width: 200
        height: 160
        visible: ebutton.mssz === 3  // 红外模式下常显，点击后不消失
        z: 1000
        hoverEnabled: false

        onClicked: {
            console.log("画点按钮被点击！")
            showDrawPointMenu = !showDrawPointMenu  // 切换显示/隐藏
            showPseudoColorMenu = false
            showIrOptions = false
            showDrawLineMenu = false
        }

        background: Rectangle {
            color: drawPointButton.down ? "#4a6a8a" : "##1c1c1c"
            radius: 4
            border.width: 1
            border.color: "transparent"  // 完全不要边框
        }

        contentItem: Text {
            text: {
                var lang = ebutton.yuyan
                if (lang === 0) return "点"
                else if (lang === 4) return "点"
                else if (lang === 1) return "Point"
                else if (lang === 5) return "Punto"
                else return "Точка"
            }
            font.family: "Microsoft YaHei"  // 字体名称
            color: "white"
            font.bold: false
            font.pixelSize: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    // 分隔线1 - 伪彩按钮和画框按钮之间
    Rectangle {
        x: 1700
        y: 741
        width: 200
        height: 1
        color: "#3a3a3a"
        visible: ebutton.mssz === 3
        z: 1001
    }

    // 画点菜单弹窗 - 向左展开
    Rectangle {
        id: drawPointPopup
        x: 1500
        y: 500
        width: 200
        height: 230
        radius: 10
        visible: showDrawPointMenu && ebutton.mssz === 3
        z: 1001
        border.width: 2
        border.color: "#333"
        color: "white"

        Column {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            Button {
                width: parent.width - 20
                height: 70
                font.pixelSize: 26

                background: Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                    border.width: 1
                    border.color: "#ccc"
                }
                contentItem: Text {
                    text: {
                        var lang = ebutton.yuyan
                        if (lang === 0) return "放置"
                        else if (lang === 4) return "放置"
                        else if (lang === 1) return "Place"
                        else if (lang === 5) return "Posizione"
                        else return "Положить"
                    }
                    color: "#333"
                    font.bold: true
                    font.pixelSize: 26
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    showIrPoint = true
                    pointCounter++

                    var newPtX = 960
                    var newPtY = 540
                    var offset = 100
                    var found = true
                    var attempts = 0

                    while (found && attempts < 20) {
                        found = false
                        for (var i = 0; i < pointListModel.count; i++) {
                            var pt = pointListModel.get(i)
                            if (Math.abs(pt.ptX - newPtX) < 80 && Math.abs(pt.ptY - newPtY) < 80) {
                                found = true
                                break
                            }
                        }
                        if (found) {
                            newPtX = 960 + (attempts % 5 - 2) * offset
                            newPtY = 540 + Math.floor(attempts / 5) * offset
                            attempts++
                        }
                    }

                    pointListModel.append({
                        "pointId": pointCounter,
                        "ptX": newPtX,
                        "ptY": newPtY,
                        "ptTemp": 0.0
                    })
                    showDrawPointMenu = false
                }
            }

            Button {
                width: parent.width - 20
                height: 70
                font.pixelSize: 26

                background: Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                    border.width: 1
                    border.color: "#ccc"
                }
                contentItem: Text {
                    text: {
                        var lang = ebutton.yuyan
                        if (lang === 0) return "清除"
                        else if (lang === 4) return "クリア"
                        else if (lang === 1) return "Clear"
                        else if (lang === 5) return "Cancella"
                        else return "Очистить"
                    }
                    color: "#333"
                    font.bold: true
                    font.pixelSize: 26
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    showIrPoint = false
                    pointListModel.clear()
                    pointCounter = 0
                    showDrawPointMenu = false
                }
            }

            Button {
                text: "关闭"
                width: parent.width - 20
                height: 50
                font.pixelSize: 22

                background: Rectangle {
                    color: "#e0e0e0"
                    radius: 8
                }
                contentItem: Text {
                    text: {
                        var lang = ebutton.yuyan
                        if (lang === 0) return "关闭"
                        else if (lang === 1) return "Close"
                        else if (lang === 4) return "閉じる"
                        else if (lang === 5) return "Chiudi"
                        else return "Закрыть"
                    }
                    color: "#666"
                    font.pixelSize: 22
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    showDrawPointMenu = false
                }
            }
        }
    }


    Text {
        id: maxTempText
        visible: ebutton.mssz === 3
        x: 10
        y: 150
        color: "white"
        text: {
            var lang = ebutton.yuyan
            if (lang === 0) return "最高温度: " + maxTemperature.toFixed(1) + "°C"
            else if (lang === 1) return "Tmax: " + maxTemperature.toFixed(1) + "°C"
            else if (lang === 2) return "Tmax: " + maxTemperature.toFixed(1) + "°C"
            else if (lang === 4) return "Tmax: " + maxTemperature.toFixed(1) + "°C"
            else if (lang === 5) return "Tmax: " + maxTemperature.toFixed(1) + "°C"
            else return "Tmax: " + maxTemperature.toFixed(1) + "°C"
        }
        font.pixelSize: 32
        font.bold: true
        z: 2
    }


    // 伪彩菜单弹窗
    Rectangle {
        id: pseudoColorPopup
        x: 1500
        y: 100
        width: 200
        height: 800
        radius: 15
        visible: showPseudoColorMenu && ebutton.mssz === 3
        z: 1010
        border.width: 2
        border.color: "#f5f5f5"

        // 伪彩颜色列表数据模型
        property var pseudoColorList: {
             var lang = ebutton.yuyan
             if (lang === 0) { // 中文
                 return ["白热", "黑热", "聚变", "彩虹", "金秋",
                         "午日", "铁红", "琥珀", "玉石", "夕阳",
                         "冰火", "油画", "石榴", "翡翠", "春",
                         "夏", "秋", "冬", "热检测", "极光"]
             } else if (lang === 1) { // 英文
                 return ["White Hot", "Black Hot", "Fusion", "Rainbow", "Golden Autumn",
                         "Noon", "Iron Red", "Amber", "Jade", "Sunset",
                         "Ice Fire", "Oil Painting", "Pomegranate", "Emerald", "Spring",
                         "Summer", "Autumn", "Winter", "Heat Detect", "Aurora"]
             } else if (lang === 4) { // 日文
                 return ["白熱", "黒熱", "融合", "レインボー", "金秋",
                         "正午", "鉄赤", "琥珀", "翡翠", "夕陽",
                         "氷炎", "油絵", "石榴", "ヒスイ", "春",
                         "夏", "秋", "冬", "熱検出", "オーロラ"]
             } else if (lang === 5) { // 意大利语
                 return ["Bianco Caldo", "Nero Caldo", "Fusione", "Arcobaleno", "Autunno D'oro",
                         "Mezzogiorno", "Rosso Ferro", "Ambrazione", "Giada", "Tramonto",
                         "Fuoco di Ghiaccio", "Olio Pittura", "Melograno", "Smeraldo", "Primavera",
                         "Estate", "Autunno", "Inverno", "Rilevamento Calore", "Aurora"]
             } else { // 俄语
                 return ["Белое тепло", "Черное тепло", "Сливание", "Радуга", "Золотая осень",
                         "Полдень", "Железо красное", "Янтарь", "Нефрит", "Закат",
                         "Ледяной огонь", "Масляная живопись", "Гранат", "Изумруд", "Весна",
                         "Лето", "Осень", "Зима", "Тепловое детектирование", "Аврора"]
             }
         }

        ScrollView {
            id: scrollView
            anchors.fill: parent
            anchors.margins: 5
            clip: true
            contentWidth: availableWidth

            ScrollBar.vertical: ScrollBar {
                width: 8
                policy: ScrollBar.AsNeeded
                background: Rectangle { color: "transparent" }
                contentItem: Rectangle {
                    color: "#f5f5f5"
                    radius: 4
                }
            }

            Column {
                id: pseudoColorColumn
                width: scrollView.availableWidth
                spacing: 8

                // 标题
                Text {
                    text: {
                        var lang = ebutton.yuyan
                        if (lang === 0) return "伪彩选择"
                        else if (lang === 1) return "Pseudo Color"
                        else if (lang === 4) return "擬似カラー"
                        else if (lang === 5) return "Colore Pseudo"
                        else return "Псевдоколор"
                    }
                    color: "black"
                    font.bold: true
                    font.pixelSize: 24
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                // 分隔线
                Rectangle {
                    width: parent.width - 10
                    height: 1
                    color: "#2196F3"
                }

                // 使用 Repeater 动态生成按钮
                Repeater {
                    id: pseudoColorRepeater
                    model: pseudoColorPopup.pseudoColorList

                    Button {
                        text: modelData
                        width: pseudoColorColumn.width - 10
                        height: 50
                        font.pixelSize: 25

                        background: Rectangle {
                            color: "#f5f5f5"
                            radius: 8
                        }

                        contentItem: Text {

                            text: parent.text
                            color: "black"
                            font: parent.font
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            ebutton.setIRColor(index);
                            showPseudoColorMenu = false
                        }
                    }
                }

                // 分隔线
                Rectangle {
                    width: parent.width - 10
                    height: 1
                    color: "#2196F3"
                }

                // 关闭按钮
                Button {

                    id: pseudoColorCloseBtn
                    width: pseudoColorColumn.width - 10
                    height: 50
                    font.pixelSize: 20

                    background: Rectangle {
                        color: "#ccc"
                        radius: 8
                    }

                    contentItem: Text {
                        text: {
                            var lang = ebutton.yuyan
                            if (lang === 0) return "关闭"
                            else if (lang === 1) return "Close"
                            else if (lang === 4) return "閉じる"
                            else if (lang === 5) return "Chiudi"
                            else return "Закрыть"
                        }
                        color: "white"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: {
                        showPseudoColorMenu = false
                    }
                }
            }
        }
    }

    HotColdInfo {
        id: hotColdInfo
    }

    function updateRectTemp() {
        if (showIrRectangle && ebutton.mssz === 3 && rectListModel.count > 0){
            for (var i = 0; i < rectListModel.count; i++) {
                var r = rectListModel.get(i)
                var x1 = r.rectX
                var y1 = r.rectY
                var x2 = r.rectX + r.rectW
                var y2 = r.rectY + r.rectH
                irTool.getRectTemperature(x1, y1, x2, y2, temper)
                rectListModel.setProperty(i, "maxTemp", temper.temperatureMax)
                rectListModel.setProperty(i, "hotX", temper.hotPointX)
                rectListModel.setProperty(i, "hotY", temper.hotPointY)
            }
        }
    }

    function updateHotColdPoint() {
        if (ebutton.mssz === 3) {
            irTool.getHotColdPoint(hotColdInfo)
            maxTemperature = hotColdInfo.hotTemper
            globalHotX = hotColdInfo.hotPointX
            globalHotY = hotColdInfo.hotPointY

        }
    }

    // 收集所有红外测量工具数据，返回JSON字符串供C++拍照时绘制
    function getMeasurementData() {
        var obj = {
            "showRect": showIrRectangle,
            "showLine": showIrLineTool,
            "showPoint": showIrPoint,
            "globalHotX": globalHotX,
            "globalHotY": globalHotY,
            "maxTemp": maxTemperature,
            "rects": [],
            "points": [],
            "lineX1": irLinePt1X,
            "lineY1": irLinePt1Y,
            "lineX2": irLinePt2X,
            "lineY2": irLinePt2Y,
            "lineHotX": lineTemper.hotPointX,
            "lineHotY": lineTemper.hotPointY,
            "lineMaxTemp": lineTemper.temperatureMax
        };
        for (var i = 0; i < rectListModel.count; i++) {
            var r = rectListModel.get(i);
            obj.rects.push({
                "x": r.rectX, "y": r.rectY,
                "w": r.rectW, "h": r.rectH,
                "maxTemp": r.maxTemp,
                "hotX": r.hotX, "hotY": r.hotY
            });
        }
        for (var j = 0; j < pointListModel.count; j++) {
            var p = pointListModel.get(j);
            obj.points.push({
                "x": p.ptX, "y": p.ptY,
                "temp": p.ptTemp
            });
        }
        var jsonResult = JSON.stringify(obj);
        ebutton.setIrMeasurementData(jsonResult);
        return jsonResult;
    }

    Timer {
        id: updateTimer
        interval: 100
        repeat: true
        running:ebutton.mssz === 3
        onTriggered: {
            updateRectTemp()
            updateHotColdPoint()
            getMeasurementData()
        }
    }

    // 红外模式选项弹窗 - 向左展开
    Rectangle {

        RuleTemper {
            id: temper
        }

        IRTool {
            id: irTool
        }

        id: irOptionsPopup
        x: 1500  // 向左展开，相对于按钮左侧
        y: 180
        width: 200
        height: 230

        radius: 10
        visible: showIrOptions && ebutton.mssz === 3
        z: 1001
        border.width: 2
        border.color: "#333"
        color: "white"

        Column {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            Button {
                width: parent.width - 20
                height: 70
                font.pixelSize: 26

                background: Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                    border.width: 1
                    border.color: "#ccc"
                }
                contentItem: Text {
                    text: {
                        var lang = ebutton.yuyan
                        if (lang === 0) return "放置"
                        else if (lang === 4) return "放置"
                        else if (lang === 1) return "Place"
                        else if (lang === 5) return "Posizione"
                        else return "Положить"
                    }
                    color: "#333"
                    font.bold: true
                    font.pixelSize: 26
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    showIrRectangle = true
                    rectCounter++
                    var newRectX = 660
                    var newRectY = 290
                    var newRectW = 500
                    var newRectH = 400
                    var offset = 80
                    var found = true
                    var attempts = 0
                    while (found && attempts < 20) {
                        found = false
                        for (var i = 0; i < rectListModel.count; i++) {
                            var r = rectListModel.get(i)
                            if (Math.abs(r.rectX - newRectX) < offset && Math.abs(r.rectY - newRectY) < offset) {
                                found = true
                                break
                            }
                        }
                        if (found) {
                            newRectX = 660 + (attempts % 4 - 1) * offset
                            newRectY = 290 + Math.floor(attempts / 4) * offset
                            attempts++
                        }
                    }
                    rectListModel.append({
                        "rectId": rectCounter,
                        "rectX": newRectX,
                        "rectY": newRectY,
                        "rectW": newRectW,
                        "rectH": newRectH,
                        "maxTemp": 0.0,
                        "hotX": 0,
                        "hotY": 0
                    })
                    showIrOptions = false
                }
            }

            Button {
                width: parent.width - 20
                height: 70
                font.pixelSize: 26

                background: Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                    border.width: 1
                    border.color: "#ccc"
                }
                contentItem: Text {
                    text: {
                        var lang = ebutton.yuyan
                        if (lang === 0) return "清除"
                        else if(lang === 4) return "クリア"
                        else if (lang === 1) return "Clear"
                        else if (lang === 5) return "Cancella"
                        else return "Очистить"
                    }
                    color: "#333"
                    font.bold: true
                    font.pixelSize: 26
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    showIrRectangle = false
                    showIrOptions = false
                    rectListModel.clear()
                }
            }

            Button {
                width: parent.width - 20
                height: 50
                font.pixelSize: 22

                background: Rectangle {
                    color: "#e0e0e0"
                    radius: 8
                }
                contentItem: Text {
                    text: {
                        var lang = ebutton.yuyan
                        if (lang === 0) return "关闭"
                        else if (lang === 1) return "Close"
                        else if (lang === 4) return "閉じる"
                        else if (lang === 5) return "Chiudi"
                        else return "Закрыть"
                    }
                    color: "#666"
                    font.pixelSize: 22
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    showIrOptions = false
                }
            }
        }
    }

    Item {
        id: rectContainer
        x: 0
        y: 0
        width: 1920
        height: 1080
        visible: showIrRectangle && ebutton.mssz === 3
        z: 200

        Repeater {
            model: rectListModel

            Item {
                id: rectItem
                x: model.rectX
                y: model.rectY
                width: model.rectW
                height: model.rectH

                property int minSize: 100
                property int boundMinX: 285
                property int boundMaxX: 1920 - 285
                property int boundMinY: 0
                property int boundMaxY: 1080
                property int idx: index

                function clampX(val) {
                    return Math.max(boundMinX, Math.min(boundMaxX - rectItem.width, val))
                }
                function clampY(val) {
                    return Math.max(boundMinY, Math.min(boundMaxY - rectItem.height, val))
                }

                function syncToModel() {
                    rectListModel.setProperty(idx, "rectX", rectItem.x)
                    rectListModel.setProperty(idx, "rectY", rectItem.y)
                    rectListModel.setProperty(idx, "rectW", rectItem.width)
                    rectListModel.setProperty(idx, "rectH", rectItem.height)
                }

                Canvas {
                    id: hotPointTriangle
                    anchors.fill: parent
                    z: 200
                    property int localHotX: model.hotX - rectItem.x
                    property int localHotY: model.hotY - rectItem.y
                    onLocalHotXChanged: requestPaint()
                    onLocalHotYChanged: requestPaint()
                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)
                        var px = localHotX
                        var py = localHotY
                        var size = 20
                        var margin = size + 5

                        if (px >= 0 && px <= width && py >= 0 && py <= height) {
                            var drawX = Math.max(margin, Math.min(width - margin, px))
                            var drawY = Math.max(margin, Math.min(height - margin, py))

                            ctx.beginPath()
                            ctx.moveTo(drawX, drawY - size)
                            ctx.lineTo(drawX - size * 0.6, drawY + size * 0.5)
                            ctx.lineTo(drawX + size * 0.6, drawY + size * 0.5)
                            ctx.closePath()
                            ctx.fillStyle = "red"
                            ctx.fill()
                        }
                    }
                }
                // 主矩形框
                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.width: 2
                    border.color: "#4CAF50"
                    smooth: true
                    opacity: 0.9
                    // 内部拖拽区域
                    MouseArea {
                        anchors.fill: parent
                        drag.target: rectItem
                        drag.minimumX: 285
                        drag.maximumX: 1920 - 287 - rectItem.width
                        drag.minimumY: 0
                        drag.maximumY: 1080 - 2 - rectItem.height
                        cursorShape: Qt.SizeAllCursor

                        onPositionChanged: {
                            if (drag.active) {
                                syncToModel()
                            }
                        }
                    }
                }

                Text {
                    x: 5
                    y: 5
                    text: "Max: " + model.maxTemp.toFixed(1) + "°C"
                    color: "white"
                    font.pixelSize: 32
                    font.bold: true
                    style: Text.Outline
                    styleColor: "black"
                    z: 15
                }
                // 左上角手柄
                Rectangle {
                    x: -15
                    y: -15
                    width: 30
                    height: 30
                    color: "#4CAF50"
                    radius: 8
                    border.width: 1
                    border.color: "#FFFFFF"
                    smooth: true

                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: -15
                        cursorShape: Qt.SizeFDiagCursor
                        preventStealing: true

                        property real startGlobalX: 0
                        property real startGlobalY: 0
                        property rect startRect: Qt.rect(0, 0, 0, 0)

                        onPressed: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            startGlobalX = globalPos.x
                            startGlobalY = globalPos.y
                            startRect = Qt.rect(rectItem.x, rectItem.y, rectItem.width, rectItem.height)
                            mouse.accepted = true
                        }

                        onPositionChanged: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            var dx = globalPos.x - startGlobalX
                            var dy = globalPos.y - startGlobalY
                            var newW = Math.max(rectItem.minSize, startRect.width - dx)
                            var newH = Math.max(rectItem.minSize, startRect.height - dy)
                            var newX = startRect.x + (startRect.width - newW)
                            var newY = startRect.y + (startRect.height - newH)
                            newX = rectItem.clampX(newX)
                            newY = rectItem.clampY(newY)
                            rectItem.x = newX
                            rectItem.y = newY
                            rectItem.width = newW
                            rectItem.height = newH
                            syncToModel()
                        }
                    }
                }
                // 右上角手柄
                Rectangle {
                    x: parent.width - 15
                    y: -15
                    width: 30
                    height: 30
                    color: "#4CAF50"
                    radius: 8
                    border.width: 1
                    border.color: "#FFFFFF"
                    smooth: true

                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: -15
                        cursorShape: Qt.SizeBDiagCursor
                        preventStealing: true

                        property real startGlobalX: 0
                        property real startGlobalY: 0
                        property rect startRect: Qt.rect(0, 0, 0, 0)

                        onPressed: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            startGlobalX = globalPos.x
                            startGlobalY = globalPos.y
                            startRect = Qt.rect(rectItem.x, rectItem.y, rectItem.width, rectItem.height)
                            mouse.accepted = true
                        }

                        onPositionChanged: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            var dx = globalPos.x - startGlobalX
                            var dy = globalPos.y - startGlobalY
                            var newW = Math.max(rectItem.minSize, startRect.width + dx)
                            var newH = Math.max(rectItem.minSize, startRect.height - dy)
                            var newY = startRect.y + (startRect.height - newH)
                            newY = rectItem.clampY(newY)
                            rectItem.y = newY
                            rectItem.width = Math.min(newW, rectItem.boundMaxX - rectItem.x)
                            rectItem.height = newH
                            syncToModel()
                        }
                    }
                }
                // 左下角手柄
                Rectangle {
                    x: -15
                    y: parent.height - 15
                    width: 30
                    height: 30
                    color: "#4CAF50"
                    radius: 8
                    border.width: 1
                    border.color: "#FFFFFF"
                    smooth: true

                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: -15
                        cursorShape: Qt.SizeBDiagCursor
                        preventStealing: true

                        property real startGlobalX: 0
                        property real startGlobalY: 0
                        property rect startRect: Qt.rect(0, 0, 0, 0)

                        onPressed: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            startGlobalX = globalPos.x
                            startGlobalY = globalPos.y
                            startRect = Qt.rect(rectItem.x, rectItem.y, rectItem.width, rectItem.height)
                            mouse.accepted = true
                        }

                        onPositionChanged: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            var dx = globalPos.x - startGlobalX
                            var dy = globalPos.y - startGlobalY
                            var newW = Math.max(rectItem.minSize, startRect.width - dx)
                            var newH = Math.max(rectItem.minSize, startRect.height + dy)
                            var newX = startRect.x + (startRect.width - newW)
                            newX = rectItem.clampX(newX)
                            rectItem.x = newX
                            rectItem.width = newW
                            rectItem.height = Math.min(newH, rectItem.boundMaxY - rectItem.y)
                            syncToModel()
                        }
                    }
                }
                // 右下角手柄
                Rectangle {
                    x: parent.width - 15
                    y: parent.height - 15
                    width: 30
                    height: 30
                    color: "#4CAF50"
                    radius: 8
                    border.width: 1
                    border.color: "#FFFFFF"
                    smooth: true

                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: -15
                        cursorShape: Qt.SizeFDiagCursor
                        preventStealing: true

                        property real startGlobalX: 0
                        property real startGlobalY: 0
                        property rect startRect: Qt.rect(0, 0, 0, 0)

                        onPressed: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            startGlobalX = globalPos.x
                            startGlobalY = globalPos.y
                            startRect = Qt.rect(rectItem.x, rectItem.y, rectItem.width, rectItem.height)
                            mouse.accepted = true
                        }

                        onPositionChanged: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            var dx = globalPos.x - startGlobalX
                            var dy = globalPos.y - startGlobalY
                            var newW = Math.max(rectItem.minSize, startRect.width + dx)
                            var newH = Math.max(rectItem.minSize, startRect.height + dy)
                            rectItem.width = Math.min(newW, rectItem.boundMaxX - rectItem.x)
                            rectItem.height = Math.min(newH, rectItem.boundMaxY - rectItem.y)
                            syncToModel()
                        }
                    }
                }
                // 左边手柄
                Rectangle {
                    x: -10
                    y: parent.height / 2 - 15
                    width: 20
                    height: 30
                    color: "#4CAF50"
                    radius: 4
                    border.width: 1
                    border.color: "#FFFFFF"
                    smooth: true

                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: -15
                        cursorShape: Qt.SizeHorCursor
                        preventStealing: true

                        property real startGlobalX: 0
                        property rect startRect: Qt.rect(0, 0, 0, 0)

                        onPressed: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            startGlobalX = globalPos.x
                            startRect = Qt.rect(rectItem.x, rectItem.y, rectItem.width, rectItem.height)
                            mouse.accepted = true
                        }

                        onPositionChanged: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            var dx = globalPos.x - startGlobalX
                            var newW = Math.max(rectItem.minSize, startRect.width - dx)
                            var newX = startRect.x + (startRect.width - newW)
                            newX = rectItem.clampX(newX)
                            rectItem.x = newX
                            rectItem.width = newW
                            syncToModel()
                        }
                    }
                }
                // 右边手柄
                Rectangle {
                    x: parent.width - 10
                    y: parent.height / 2 - 15
                    width: 20
                    height: 30
                    color: "#4CAF50"
                    radius: 4
                    border.width: 1
                    border.color: "#FFFFFF"
                    smooth: true

                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: -15
                        cursorShape: Qt.SizeHorCursor
                        preventStealing: true

                        property real startGlobalX: 0
                        property rect startRect: Qt.rect(0, 0, 0, 0)

                        onPressed: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            startGlobalX = globalPos.x
                            startRect = Qt.rect(rectItem.x, rectItem.y, rectItem.width, rectItem.height)
                            mouse.accepted = true
                        }

                        onPositionChanged: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            var dx = globalPos.x - startGlobalX
                            var newW = Math.max(rectItem.minSize, startRect.width + dx)
                            rectItem.width = Math.min(newW, rectItem.boundMaxX - rectItem.x)
                            syncToModel()
                        }
                    }
                }
                // 上边手柄
                Rectangle {
                    x: parent.width / 2 - 15
                    y: -10
                    width: 30
                    height: 20
                    color: "#4CAF50"
                    radius: 4
                    border.width: 1
                    border.color: "#FFFFFF"
                    smooth: true

                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: -15
                        cursorShape: Qt.SizeVerCursor
                        preventStealing: true

                        property real startGlobalY: 0
                        property rect startRect: Qt.rect(0, 0, 0, 0)

                        onPressed: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            startGlobalY = globalPos.y
                            startRect = Qt.rect(rectItem.x, rectItem.y, rectItem.width, rectItem.height)
                            mouse.accepted = true
                        }

                        onPositionChanged: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            var dy = globalPos.y - startGlobalY
                            var newH = Math.max(rectItem.minSize, startRect.height - dy)
                            var newY = startRect.y + (startRect.height - newH)
                            newY = rectItem.clampY(newY)
                            rectItem.y = newY
                            rectItem.height = newH
                            syncToModel()
                        }
                    }
                }
                // 下边手柄
                Rectangle {
                    x: parent.width / 2 - 15
                    y: parent.height - 10
                    width: 30
                    height: 20
                    color: "#4CAF50"
                    radius: 4
                    border.width: 1
                    border.color: "#FFFFFF"
                    smooth: true

                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: -15
                        cursorShape: Qt.SizeVerCursor
                        preventStealing: true

                        property real startGlobalY: 0
                        property rect startRect: Qt.rect(0, 0, 0, 0)

                        onPressed: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            startGlobalY = globalPos.y
                            startRect = Qt.rect(rectItem.x, rectItem.y, rectItem.width, rectItem.height)
                            mouse.accepted = true
                        }

                        onPositionChanged: {
                            var globalPos = mapToItem(null, mouseX, mouseY)
                            var dy = globalPos.y - startGlobalY
                            var newH = Math.max(rectItem.minSize, startRect.height + dy)
                            rectItem.height = Math.min(newH, rectItem.boundMaxY - rectItem.y)
                            syncToModel()
                        }
                    }
                }
            }
        }
    }


    // 全局最高温点三角标记
    Canvas {
        id: globalHotPointCanvas
        x: 0
        y: 0
        width: 1920
        height: 1080
        visible: ebutton.mssz === 3
        z: 250
        property int hotX: globalHotX
        property int hotY: globalHotY
        onHotXChanged: requestPaint()
        onHotYChanged: requestPaint()
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            var px = hotX
            var py = hotY
            var size = 20
            var margin = size + 5
            // 限定在有效显示区域内
            if (px >= 285 && px <= 1920 - 287 && py >= 5 && py <= 1075) {
                var drawX = Math.max(285 + margin, Math.min(1920 - 287 - margin, px))
                var drawY = Math.max(margin, Math.min(1080 - margin, py))
                ctx.beginPath()
                ctx.moveTo(drawX, drawY - size)
                ctx.lineTo(drawX - size * 0.6, drawY + size * 0.5)
                ctx.lineTo(drawX + size * 0.6, drawY + size * 0.5)
                ctx.closePath()
                ctx.fillStyle = "red"
                ctx.fill()
            }
        }
    }

    // // 在顶层定义直线相关属性
    property int irLinePt1X: 400
    property int irLinePt1Y: 300
    property int irLinePt2X: 600
    property int irLinePt2Y: 500
    property int irLineMidPtX: (irLinePt1X + irLinePt2X)/2
    property int irLineMidPtY: (irLinePt1Y + irLinePt2Y) / 2
    property bool showIrLineTool: false
    property bool irLineIsRotating: false
    property bool irLineIsDragging: false
    property int lastLineUpdateTick: 0

    function updateLineTempThrottled() {
        var now = Date.now()
        if (now - lastLineUpdateTick > 150) {
            lastLineUpdateTick = now
            irTool.getLineTemperature(irLinePt1X, irLinePt1Y, irLinePt2X, irLinePt2Y, lineTemper)
        }
    }


    RuleTemper {
        id: lineTemper
    }

    // 直线温度更新定时器
    Timer {
        id: lineUpdateTimer
        interval: 100
        running: showIrLineTool && ebutton.mssz === 3
        repeat: true
        onTriggered: {
            if (showIrLineTool && ebutton.mssz === 3) {
                irTool.getLineTemperature(irLinePt1X, irLinePt1Y, irLinePt2X, irLinePt2Y, lineTemper)
                getMeasurementData()
            }
        }
    }

    // 可拖拽旋转的直线
    Item {
        id: irLineContainer
        x: 0
        y: 0
        width: 1920  // 设置足够大的绘制区域
        height: 1080
        visible: showIrLineTool && ebutton.mssz === 3
        z: 200

        // 主直线Canvas
        Canvas {
            id: lineCanvas
            anchors.fill: parent
            z: 10

            // 设置渲染策略
            renderStrategy: Canvas.CooperativeRenderStrategy
            renderTarget: Canvas.FramebufferObject
            smooth: true
            antialiasing: true

            property int hotX: lineTemper.hotPointX
            property int hotY: lineTemper.hotPointY
            onHotXChanged: requestPaint()
            onHotYChanged: requestPaint()

            onPaint: {
                var ctx = getContext("2d")

                // 完全清除画布
                ctx.clearRect(0, 0, width, height)

                // 绘制直线
                ctx.beginPath()
                ctx.moveTo(irLinePt1X, irLinePt1Y)
                ctx.lineTo(irLinePt2X, irLinePt2Y)
                ctx.strokeStyle = "#4CAF50"
                ctx.lineWidth = 3
                ctx.stroke()

                // 绘制端点标记
                ctx.fillStyle = "#4CAF50"
                ctx.beginPath()
                ctx.arc(irLinePt1X, irLinePt1Y, 5, 0, Math.PI * 2)
                ctx.fill()

                ctx.beginPath()
                ctx.arc(irLinePt2X, irLinePt2Y, 5, 0, Math.PI * 2)
                ctx.fill()

                // 绘制中点标记
                var midX = (irLinePt1X + irLinePt2X) / 2
                var midY = (irLinePt1Y + irLinePt2Y) / 2
                ctx.fillStyle = "#FF9800"
                ctx.beginPath()
                ctx.arc(midX, midY, 5, 0, Math.PI * 2)
                ctx.fill()

                // 绘制最高温三角形
                var px = hotX
                var py = hotY
                var size = 20
                ctx.beginPath()
                ctx.moveTo(px, py)
                ctx.lineTo(px - size * 0.6, py + size)
                ctx.lineTo(px + size * 0.6, py + size)
                ctx.closePath()
                ctx.fillStyle = "red"
                ctx.fill()

                // 绘制温度文本
                var tempText = lineTemper.temperatureMax.toFixed(1) + "°C"
                ctx.font = "bold 32px sans-serif"
                ctx.textAlign = "center"
                ctx.fillStyle = "white"
                ctx.strokeStyle = "black"
                ctx.lineWidth = 3
                ctx.strokeText(tempText, px, py + size + 28)
                ctx.fillText(tempText, px, py + size + 28)
            }
        }

        // 左端点节点 - 可拖拽改变位置和长度
        Rectangle {
            id: lineNode1
            x: irLinePt1X - 20
            y: irLinePt1Y - 20
            width: 30
            height: 30
            color: "#4CAF50"
            radius: 8
            border.width: 2
            border.color: "#FFFFFF"
            smooth: true

            MouseArea {
                anchors.fill: parent
                anchors.margins: -10
                cursorShape: Qt.CrossCursor

                property point startGlobalPos: Qt.point(0, 0)
                property int startX1: 0
                property int startY1: 0

                onPressed: {
                    startGlobalPos = mapToItem(irLineContainer, mouseX, mouseY)
                    startX1 = irLinePt1X
                    startY1 = irLinePt1Y
                    irLineIsRotating = true
                    mouse.accepted = true
                }

                onPositionChanged: {
                    if (irLineIsRotating) {
                        var currentGlobalPos = mapToItem(irLineContainer, mouseX, mouseY)
                        var dx = currentGlobalPos.x - startGlobalPos.x
                        var dy = currentGlobalPos.y - startGlobalPos.y

                        irLinePt1X = Math.max(285, Math.min(1920 - 287, startX1 + dx))
                        irLinePt1Y = Math.max(20, Math.min(1080 - 20, startY1 + dy))
                        irLineMidPtX = (irLinePt1X + irLinePt2X) / 2
                        irLineMidPtY = (irLinePt1Y + irLinePt2Y) / 2

                        updateLineTempThrottled()
                        lineCanvas.requestPaint()
                    }
                }

                onReleased: {
                    irLineIsRotating = false
                    irTool.getLineTemperature(irLinePt1X, irLinePt1Y, irLinePt2X, irLinePt2Y, lineTemper)
                    lineCanvas.requestPaint()
                }
            }
        }


        // 中间节点 - 可拖动
        Rectangle {
            id: lineNodeMid
            x: irLineMidPtX - 20
            y: irLineMidPtY - 20
            width: 30
            height: 30
            color: "#FF9800"
            radius: 8
            border.width: 2
            border.color: "#FFFFFF"
            smooth: true

            MouseArea {
                anchors.fill: parent
                anchors.margins: -10
                cursorShape: Qt.SizeAllCursor

                property point startGlobalPos: Qt.point(0, 0)
                property int startX1: 0
                property int startY1: 0
                property int startX2: 0
                property int startY2: 0

                onPressed: {
                    startGlobalPos = mapToItem(irLineContainer, mouseX, mouseY)
                    startX1 = irLinePt1X
                    startY1 = irLinePt1Y
                    startX2 = irLinePt2X
                    startY2 = irLinePt2Y
                    irLineIsDragging = true
                    mouse.accepted = true
                }

                onPositionChanged: {
                    if (irLineIsDragging) {
                        var currentGlobalPos = mapToItem(irLineContainer, mouseX, mouseY)
                        var dx = currentGlobalPos.x - startGlobalPos.x
                        var dy = currentGlobalPos.y - startGlobalPos.y

                        irLinePt1X = Math.max(285, Math.min(1920 - 287, startX1 + dx))
                        irLinePt1Y = Math.max(20, Math.min(1080 - 20, startY1 + dy))
                        irLinePt2X = Math.max(285, Math.min(1920 - 287, startX2 + dx))
                        irLinePt2Y = Math.max(20, Math.min(1080 - 20, startY2 + dy))
                        irLineMidPtX = (irLinePt1X + irLinePt2X) / 2
                        irLineMidPtY = (irLinePt1Y + irLinePt2Y) / 2

                        updateLineTempThrottled()
                        lineCanvas.requestPaint()
                    }
                }

                onReleased: {
                    irLineIsDragging = false
                    irTool.getLineTemperature(irLinePt1X, irLinePt1Y, irLinePt2X, irLinePt2Y, lineTemper)
                    lineCanvas.requestPaint()
                }
            }
        }

        // 右端点节点 - 可拖拽改变位置和长度
        Rectangle {
            id: lineNode2
            x: irLinePt2X - 20
            y: irLinePt2Y - 20
            width: 30
            height: 30
            color: "#4CAF50"
            radius: 8
            border.width: 2
            border.color: "#FFFFFF"
            smooth: true

            MouseArea {
                anchors.fill: parent
                anchors.margins: -10
                cursorShape: Qt.CrossCursor

                property point startGlobalPos: Qt.point(0, 0)
                property int startX2: 0
                property int startY2: 0

                onPressed: {
                    startGlobalPos = mapToItem(irLineContainer, mouseX, mouseY)
                    startX2 = irLinePt2X
                    startY2 = irLinePt2Y
                    irLineIsRotating = true
                    mouse.accepted = true
                }

                onPositionChanged: {
                    if (irLineIsRotating) {
                        var currentGlobalPos = mapToItem(irLineContainer, mouseX, mouseY)
                        var dx = currentGlobalPos.x - startGlobalPos.x
                        var dy = currentGlobalPos.y - startGlobalPos.y

                        irLinePt2X = Math.max(285, Math.min(1920 - 287, startX2 + dx))
                        irLinePt2Y = Math.max(20, Math.min(1080 - 20, startY2 + dy))
                        irLineMidPtX = (irLinePt1X + irLinePt2X) / 2
                        irLineMidPtY = (irLinePt1Y + irLinePt2Y) / 2

                        updateLineTempThrottled()
                        lineCanvas.requestPaint()
                    }
                }

                onReleased: {
                    irLineIsRotating = false
                    irTool.getLineTemperature(irLinePt1X, irLinePt1Y, irLinePt2X, irLinePt2Y, lineTemper)
                    lineCanvas.requestPaint()
                }
            }
        }

        // 同步中点属性
        onVisibleChanged: {
            if (visible) {
                // 强制刷新中点坐标
                irLineMidPtX = (irLinePt1X + irLinePt2X) / 2
                irLineMidPtY = (irLinePt1Y + irLinePt2Y) / 2
                lineCanvas.requestPaint()  // 强制重绘
                console.log("直线显示，中点更新：", irLineMidPtX, irLineMidPtY)
            }
        }
    }

    // 画点功能 - 温度更新定时器
    Timer {
        id: pointTempTimer
        interval: 500
        running: showIrPoint && pointListModel.count > 0 && ebutton.mssz === 3
        repeat: true
        onTriggered: {
            for (var i = 0; i < pointListModel.count; i++) {
                var pt = pointListModel.get(i)
                var temp = irTool.getPointTemp(pt.ptX, pt.ptY)
                pointListModel.setProperty(i, "ptTemp", temp)
            }
            getMeasurementData()
        }
    }

    // 画点功能 - 点的可视化容器
    Item {
        id: pointContainer
        x: 0
        y: 0
        width: 1920
        height: 1080
        visible: showIrPoint && ebutton.mssz === 3
        z: 200

        Repeater {
            model: pointListModel

            Item {
                id: pointItem
                x: model.ptX - 20
                y: model.ptY - 20
                width: 40
                height: 40

                Rectangle {
                    id: pointCircle
                    anchors.centerIn: parent
                    width: 30
                    height: 30
                    radius: 15
                    color: "#FF5722"
                    border.width: 2
                    border.color: "#FFFFFF"
                    smooth: true
                }

                Text {
                    id: pointTempText
                    x: 0
                    y: 35
                    text: model.ptTemp.toFixed(1) + "°C"
                    color: "white"
                    font.pixelSize: 24
                    font.bold: true
                    style: Text.Outline
                    styleColor: "black"
                }

                MouseArea {
                    anchors.fill: parent
                    anchors.margins: -10
                    cursorShape: Qt.SizeAllCursor
                    drag.target: pointItem
                    drag.minimumX: 285
                    drag.maximumX: 1920 - 320
                    drag.minimumY: 20
                    drag.maximumY: 1080 - 20

                    property int startX: 0
                    property int startY: 0

                    onPressed: {
                        startX = model.ptX
                        startY = model.ptY
                    }

                    onPositionChanged: {
                        if (drag.active) {
                            var newX = pointItem.x + 20
                            var newY = pointItem.y + 20
                            pointListModel.setProperty(index, "ptX", newX)
                            pointListModel.setProperty(index, "ptY", newY)
                        }
                    }
                }
            }
        }
    }

      //end
}
