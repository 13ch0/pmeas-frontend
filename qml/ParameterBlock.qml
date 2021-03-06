import QtQuick 2.7
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.1

import Theme 1.0

Rectangle {
    id: parameterBlock;
    color: "transparent";
    radius: 6;

    height: 100;
    width: 200;

    property bool mouseEntered: false;

    property int maxHeight: -1;
    property int maxWidth: -1;

    onWidthChanged: {
        if ( maxWidth > -1 ) {
            if ( width > maxWidth ) {
                width = maxWidth;
            }
        }
    }

    border {
        width: 3;
        color: mouseEntered ? Theme.highlighterColor : "#4a4a4a";
    }

    Behavior on border.color {
        PropertyAnimation {
            duration: 350;
            easing.type: Easing.InCubic;
        }
    }


    MouseArea {
        anchors.fill: parent;
        hoverEnabled: true;
        onEntered: {
            mouseEntered = true;
            textValue.forceActiveFocus();
        }

        onExited: {
            mouseEntered = false;
            textValue.focus = false;
        }

        z: 100;
        propagateComposedEvents: true;

        // Pass on the mouse events to the sliders below;
        onClicked: mouse.accepted = false;
        onPressed: mouse.accepted = false;
        onReleased: mouse.accepted = false;
        onDoubleClicked: mouse.accepted = false;
        onPositionChanged: mouse.accepted = false;
        onPressAndHold: mouse.accepted = false;

    }

    function broadcastJson() {
        bridge.tcpSend( effectsColumnArea.effectsListView.model.toBroadcastJson() );
    }


    ColumnLayout {
        anchors.centerIn: parent;
        spacing: 12;

    //                            Rectangle {
    //                                color: "red";
    //                                height: 50;
    //                                width: 100

    //                                anchors {
    //                                    horizontalCenter: parent.horizontalCenter;
    //                                }

            Text {
                //anchors.centerIn: parent;
                anchors {
                    horizontalCenter: parent.horizontalCenter;
                }

                text: parameterName;
                font {
                    pixelSize: 15;
                    bold: true;
                }

                color: "#f1f1f1";
            }

       //}

        Slider {
            id: parameterSlider;
            from: parameterMinValue;
            to: parameterMaxValue;
            value: parameterValue;
            height: 17;
            implicitWidth: parameterBlock.width * 0.75;
            anchors {
                horizontalCenter: parent.horizontalCenter;
            }
            stepSize: parameterMinValue / parameterMaxValue;
            hoverEnabled: true;
            onHoveredChanged: {
                console.log(hovered)
            }

            onValueChanged: {
                if(tutorialTip.visible && tutorialState === 3) {
                    tutorialText.text = "Very good. Changing the sliders allows you to fine tune\n" +
                            "how the effect will modulate the audio.";
                    tutorialTip.width = 400;
                    tutorialNext.visible = true;
                }
            }

            onPressedChanged: {
                if ( !pressed ) {
                    parameterValue = value;
                    textValue.tempString = value.toFixed( 2 );
                    broadcastJson();
                    textValue.forceActiveFocus();
                }
            }


            background: Rectangle {
                      x: parameterSlider.leftPadding
                      y: parameterSlider.topPadding + parameterSlider.availableHeight / 2 - height / 2
                      implicitWidth: 200
                      implicitHeight: 4
                      width: parameterSlider.availableWidth
                      height: implicitHeight
                      radius: 2
                      color: "#7a7a7a"

                      Rectangle {
                          width: parameterSlider.visualPosition * parent.width
                          height: parent.height
                          color: mouseEntered ? Theme.highlighterColor : "#bdbebf";
                          radius: 2
                      }
                  }

            handle: Item {
                      x: parameterSlider.leftPadding + parameterSlider.visualPosition * (parameterSlider.availableWidth - width)
                      y: parameterSlider.topPadding + parameterSlider.availableHeight / 2 - height / 2
                      implicitWidth: 21;
                      implicitHeight: 21;

                      Rectangle {
                          id: handleBackground;
                          anchors.fill: parent;
                          radius: height / 2;
                      }

                      DropShadow {
                          anchors.fill: source;
                          horizontalOffset: 0;
                          verticalOffset: mouseEntered ? 2 : 0;
                          radius: mouseEntered ? 10.0 : 2.0;
                          samples: radius * 2;
                          color: "black";
                          source: handleBackground;

                          Behavior on verticalOffset {
                              PropertyAnimation {
                                  duration: 500;
                                  easing.type: Easing.InCurve;
                              }
                          }

                          cached: true;


                      }

                  }

        }

        Rectangle {
            height: 25;
            width: height;
            color: "transparent";
            border {
                width: 0;
                color: "transparent";
            }

            radius: width / 2;
            anchors {
                horizontalCenter: parent.horizontalCenter;
            }

            Text {
                id: textValue;
                anchors { centerIn: parent; }
                text: tempString;
                color: mouseEntered ? "#f1f1f1" : "#919191";
                font {
                    pixelSize: 14;
                    bold: true;
                }

                property string tempString: parameterSlider.value.toFixed( 2 );
                property bool periodUsed: false;

                Keys.onPressed: {
                    switch( event.key ) {

                        case Qt.Key_Period:
                            if ( !periodUsed ) {
                                if ( tempString[ 0 ] === "?" ) {
                                    tempString = "0.";
                                } else {
                                    tempString += '.';
                                }
                                periodUsed = true;
                            }
                            break;

                        case Qt.Key_0:
                        case Qt.Key_1:
                        case Qt.Key_2:
                        case Qt.Key_3:
                        case Qt.Key_4:
                        case Qt.Key_5:
                        case Qt.Key_6:
                        case Qt.Key_7:
                        case Qt.Key_8:
                        case Qt.Key_9:

                            if ( tempString[ 0 ] === "?" ) {
                                tempString = "";
                            }

                            tempString += event.key - Qt.Key_0;
                            var f = parseFloat( tempString );
                            if ( f > parameterSlider.to ) {
                                f = parameterSlider.to
                                tempString = f.toString();
                            }

                            parameterValue = f;

                            broadcastJson();
                            break;

                        case Qt.Key_Backspace:
                            periodUsed = false;
                            tempString = "?";

                            parameterValue = parameterMinValue;
                            broadcastJson();
                            break;

                        default:
                            break;
                    }
                }

                Behavior on color {
                    PropertyAnimation {
                        duration: 350;
                        easing.type: Easing.InCubic;
                    }
                }
            }
        }

    }
}
