/*-
 * Copyright (c) 2014 Peter Tworek
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.ytplayer 1.0
import "../common"

Page {
    property string videoId

    Component.onCompleted: {
        discovery.start();
    }

    SilicaListView {
        anchors.fill: parent

        header: PageHeader {
            //: Title of play in page
            //% "Play in"
            title: qsTrId("ytplayer-title-play-in")
        }

        YTPlayerDiscovery {
            id: discovery
            model: players
        }

        model: YTListModel {
            id: players
        }

        delegate: ListItem {
            id : item
            width: parent.width

            Image {
                id: image
                height: 70
                width: 70
                fillMode: Image.PreserveAspectFit
                source: "file://" + icon
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: Theme.paddingMedium
                }
            }

            Label {
                id: itemLabel
                text: name
                color: item.highlighted ? Theme.highlightColor : Theme.primaryColor
                elide: Text.ElideRight
                maximumLineCount: 2
                wrapMode: Text.Wrap
                anchors {
                    left: image.right
                    right: parent.right
                    leftMargin: Theme.paddingSmall
                    rightMargin: Theme.paddingSmall
                    verticalCenter: parent.verticalCenter
                }
                font {
                    family: Theme.fontFamily
                    pixelSize: Theme.fontSizeExtraSmall
                }
            }

            onClicked: {
                discovery.playIn(serviceName, videoId);
                pageStack.pop();
            }
        }
    }
}
