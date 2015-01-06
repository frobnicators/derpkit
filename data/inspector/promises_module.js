WebInspector.PromisePane=function()
{WebInspector.VBox.call(this);this.registerRequiredCSS("promises/promisePane.css");this.element.classList.add("promises");this._enabled=false;this._target=null;this._dataGridContainer=new WebInspector.VBox();this._dataGridContainer.show(this.element);var columns=[{id:"status",weight:1},{id:"function",title:WebInspector.UIString("Function"),disclosure:true,weight:10},{id:"created",title:WebInspector.UIString("Created"),weight:10},{id:"settled",title:WebInspector.UIString("Settled"),weight:10},{id:"tts",title:WebInspector.UIString("Time to settle"),weight:10}];this._dataGrid=new WebInspector.DataGrid(columns,undefined,undefined,undefined,this._onContextMenu.bind(this));this._dataGrid.show(this._dataGridContainer.element);this._linkifier=new WebInspector.Linkifier();this._throttler=new WebInspector.Throttler(1000);this._popoverHelper=new WebInspector.PopoverHelper(this.element,this._getPopoverAnchor.bind(this),this._showPopover.bind(this),this._onHidePopover.bind(this));this._popoverHelper.setTimeout(250,250);this.element.addEventListener("click",this._hidePopover.bind(this),true);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.DebuggerPaused,this._debuggerStateChanged,this);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.DebuggerResumed,this._debuggerStateChanged,this);WebInspector.context.addFlavorChangeListener(WebInspector.Target,this._targetChanged,this);WebInspector.targetManager.observeTargets(this);}
WebInspector.PromisePane._detailsSymbol=Symbol("details");WebInspector.PromisePane.prototype={targetAdded:function(target)
{if(!this._enabled)
return;this._enablePromiseTracker(target);},targetRemoved:function(target)
{if(!this._enabled)
return;if(this._target===target){this._clear();this._target=null;}},_targetChanged:function(event)
{if(!this._enabled)
return;var target=(event.data);if(this._target===target)
return;this._clear();this._target=target;this._scheduleDataUpdate(true);},wasShown:function()
{if(!this._enabled){this._enabled=true;this._target=WebInspector.context.flavor(WebInspector.Target);WebInspector.targetManager.targets().forEach(this._enablePromiseTracker,this);}
this._scheduleDataUpdate(true);},willHide:function()
{this._hidePopover();},_hidePopover:function()
{this._popoverHelper.hidePopover();},_onHidePopover:function()
{this._scheduleDataUpdate(true);},_enablePromiseTracker:function(target)
{target.debuggerAgent().enablePromiseTracker(true);},_debuggerStateChanged:function()
{this._hidePopover();this._scheduleDataUpdate(true);},_comparePromises:function(p1,p2)
{var t1=p1.creationTime||0;var t2=p2.creationTime||0;return(t1-t2)||(p1.id-p2.id);},_updateData:function(finishCallback)
{var target=this._target;if(!target||this._popoverHelper.isPopoverVisible()||this._popoverHelper.isHoverTimerActive()){didUpdate.call(this);return;}
target.heapProfilerAgent().collectGarbage();target.debuggerAgent().getPromises(didGetPromises.bind(this));function didGetPromises(error,promiseDetails)
{if(target!==this._target||this._popoverHelper.isPopoverVisible()||this._popoverHelper.isHoverTimerActive()){didUpdate.call(this);return;}
var expandedState=this._dataGridExpandedState();this._clear();if(error||!promiseDetails){didUpdate.call(this);return;}
var nodesToInsert={__proto__:null};promiseDetails.sort(this._comparePromises);for(var i=0;i<promiseDetails.length;i++){var promise=promiseDetails[i];var statusElement=createElementWithClass("div","status "+promise.status);switch(promise.status){case"pending":statusElement.title=WebInspector.UIString("Pending");break;case"resolved":statusElement.title=WebInspector.UIString("Fulfilled");break;case"rejected":statusElement.title=WebInspector.UIString("Rejected");break;}
var data={status:statusElement,promiseId:promise.id,function:WebInspector.beautifyFunctionName(promise.callFrame?promise.callFrame.functionName:"")};if(promise.callFrame)
data.created=this._linkifier.linkifyConsoleCallFrame(target,promise.callFrame);if(promise.settlementStack&&promise.settlementStack[0])
data.settled=this._linkifier.linkifyConsoleCallFrame(target,promise.settlementStack[0]);if(promise.creationTime&&promise.settlementTime&&promise.settlementTime>=promise.creationTime)
data.tts=Number.millisToString(promise.settlementTime-promise.creationTime);var node=new WebInspector.DataGridNode(data,false);node.selectable=false;node[WebInspector.PromisePane._detailsSymbol]=promise;nodesToInsert[promise.id]={node:node,parentId:promise.parentId};}
var rootNode=this._dataGrid.rootNode();for(var id in nodesToInsert){var node=nodesToInsert[id].node;var parentId=nodesToInsert[id].parentId;var parentNode=(parentId&&nodesToInsert[parentId])?nodesToInsert[parentId].node:rootNode;parentNode.appendChild(node);}
for(var id in nodesToInsert){var node=nodesToInsert[id].node;node.expanded=(id in expandedState?expandedState[id]:true);}
didUpdate.call(this);}
function didUpdate()
{this._scheduleDataUpdate();finishCallback();}},_dataGridExpandedState:function()
{var result={__proto__:null};examineNode(this._dataGrid.rootNode());return result;function examineNode(node)
{var details=node[WebInspector.PromisePane._detailsSymbol];if(details)
result[details.id]=node.hasChildren?node.expanded:true;for(var child of node.children)
examineNode(child);}},_scheduleDataUpdate:function(asSoonAsPossible)
{if(!this.isShowing())
return;this._throttler.schedule(this._updateData.bind(this),asSoonAsPossible);},_clear:function()
{this._hidePopover();this._dataGrid.rootNode().removeChildren();this._linkifier.reset();},_onContextMenu:function(contextMenu,node)
{var target=this._target;if(!target)
return;var promiseId=node.data.promiseId;contextMenu.appendItem(WebInspector.UIString.capitalize("Show in ^console"),showPromiseInConsole);contextMenu.show();function showPromiseInConsole()
{target.debuggerAgent().getPromiseById(promiseId,"console",didGetPromiseById);}
function didGetPromiseById(error,promise)
{if(error||!promise)
return;target.consoleAgent().setLastEvaluationResult(promise.objectId);var message=new WebInspector.ConsoleMessage(target,WebInspector.ConsoleMessage.MessageSource.Other,WebInspector.ConsoleMessage.MessageLevel.Log,"",WebInspector.ConsoleMessage.MessageType.Log,undefined,undefined,undefined,undefined,[promise]);target.consoleModel.addMessage(message);WebInspector.console.show();}},_getPopoverAnchor:function(element,event)
{if(!this._target)
return undefined;var node=this._dataGrid.dataGridNodeFromNode(element);if(!node)
return undefined;var details=node[WebInspector.PromisePane._detailsSymbol];if(!details)
return undefined;var anchor=element.enclosingNodeOrSelfWithClass("created-column");if(anchor)
return details.creationStack?anchor:undefined;anchor=element.enclosingNodeOrSelfWithClass("settled-column");return(anchor&&details.settlementStack)?anchor:undefined;},_showPopover:function(anchor,popover)
{var node=this._dataGrid.dataGridNodeFromNode(anchor);var details=node[WebInspector.PromisePane._detailsSymbol];var stackTrace;var asyncStackTrace;if(anchor.classList.contains("created-column")){stackTrace=details.creationStack;asyncStackTrace=details.asyncCreationStack;}else{stackTrace=details.settlementStack;asyncStackTrace=details.asyncSettlementStack;}
var content=WebInspector.DOMPresentationUtils.buildStackTracePreviewContents(this._target,this._linkifier,stackTrace,asyncStackTrace);popover.setCanShrink(true);popover.showForAnchor(content,anchor);},__proto__:WebInspector.VBox.prototype};Runtime.cachedResources["promises/promisePane.css"]="/*\n * Copyright 2014 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.promises .data-grid {\n    border: none;\n    flex: 1 1;\n}\n\n.promises .data-grid th:hover {\n    background-color: inherit !important;\n}\n\n.promises .data-grid .odd {\n    background-color: #eee;\n}\n\n.promises .data-grid .header-container {\n    height: 30px;\n    border-bottom: 1px solid rgb(205, 205, 205);\n}\n\n.promises .data-grid .data-container {\n    top: 29px;\n}\n\n.promises .data-grid table.data {\n    background: transparent;\n}\n\n.promises .data-grid th {\n    border-bottom: none;\n    border-left: 1px solid rgb(205, 205, 205);\n    background-color: white;\n}\n\n.promises .data-grid td {\n    line-height: 17px;\n    height: 24px;\n    vertical-align: middle;\n    border-bottom: 1px solid rgb(205, 205, 205);\n    border-left: 1px solid rgb(205, 205, 205);\n}\n\n.promises .status {\n    -webkit-mask-image: url(Images/statusbarButtonGlyphs.png);\n    -webkit-mask-size: 320px 144px;\n    -webkit-mask-position: -294px -26px;\n    background-color: #bbb;\n    height: 20px;\n    width: 20px;\n}\n\n.promises .status.rejected {\n    background-color: rgb(216, 0, 0);\n}\n\n.promises .status.resolved {\n    background-color: #696;\n}\n\n/*# sourceURL=promises/promisePane.css */";