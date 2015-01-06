WebInspector.Spectrum=function()
{WebInspector.VBox.call(this,true);this.registerRequiredCSS("elements/spectrum.css");this.contentElement.tabIndex=0;this._draggerElement=this.contentElement.createChild("div","spectrum-color");this._dragHelperElement=this._draggerElement.createChild("div","spectrum-sat fill").createChild("div","spectrum-val fill").createChild("div","spectrum-dragger");this._sliderElement=this.contentElement.createChild("div","spectrum-hue");this.slideHelper=this._sliderElement.createChild("div","spectrum-slider");var rangeContainer=this.contentElement.createChild("div","spectrum-range-container");var alphaLabel=rangeContainer.createChild("label");alphaLabel.textContent=WebInspector.UIString("\u03B1:");this._alphaElement=rangeContainer.createChild("input","spectrum-range");this._alphaElement.setAttribute("type","range");this._alphaElement.setAttribute("min","0");this._alphaElement.setAttribute("max","100");this._alphaElement.addEventListener("input",alphaDrag.bind(this),false);this._alphaElement.addEventListener("change",alphaDrag.bind(this),false);var displayContainer=this.contentElement.createChild("div","spectrum-text");var swatchElement=displayContainer.createChild("span","swatch");this._swatchInnerElement=swatchElement.createChild("span","swatch-inner");this._displayElement=displayContainer.createChild("span","source-code spectrum-display-value");WebInspector.Spectrum.draggable(this._sliderElement,hueDrag.bind(this));WebInspector.Spectrum.draggable(this._draggerElement,colorDrag.bind(this),colorDragStart.bind(this));function hueDrag(element,dragX,dragY)
{this._hsv[0]=(this.slideHeight-dragY)/this.slideHeight;this._onchange();}
var initialHelperOffset;function colorDragStart()
{initialHelperOffset={x:this._dragHelperElement.offsetLeft,y:this._dragHelperElement.offsetTop};}
function colorDrag(element,dragX,dragY,event)
{if(event.shiftKey){if(Math.abs(dragX-initialHelperOffset.x)>=Math.abs(dragY-initialHelperOffset.y))
dragY=initialHelperOffset.y;else
dragX=initialHelperOffset.x;}
this._hsv[1]=dragX/this.dragWidth;this._hsv[2]=(this.dragHeight-dragY)/this.dragHeight;this._onchange();}
function alphaDrag()
{this._hsv[3]=this._alphaElement.value/100;this._onchange();}};WebInspector.Spectrum.Events={ColorChanged:"ColorChanged"};WebInspector.Spectrum.draggable=function(element,onmove,onstart,onstop){var dragging;var offset;var scrollOffset;var maxHeight;var maxWidth;function consume(e)
{e.consume(true);}
function move(e)
{if(dragging){var dragX=Math.max(0,Math.min(e.pageX-offset.left+scrollOffset.left,maxWidth));var dragY=Math.max(0,Math.min(e.pageY-offset.top+scrollOffset.top,maxHeight));if(onmove)
onmove(element,dragX,dragY,(e));}}
function start(e)
{var mouseEvent=(e);var rightClick=mouseEvent.which?(mouseEvent.which===3):(mouseEvent.button===2);if(!rightClick&&!dragging){if(onstart)
onstart(element,mouseEvent);dragging=true;maxHeight=element.clientHeight;maxWidth=element.clientWidth;scrollOffset=element.scrollOffset();offset=element.totalOffset();element.ownerDocument.addEventListener("selectstart",consume,false);element.ownerDocument.addEventListener("dragstart",consume,false);element.ownerDocument.addEventListener("mousemove",move,false);element.ownerDocument.addEventListener("mouseup",stop,false);move(mouseEvent);consume(mouseEvent);}}
function stop(e)
{if(dragging){element.ownerDocument.removeEventListener("selectstart",consume,false);element.ownerDocument.removeEventListener("dragstart",consume,false);element.ownerDocument.removeEventListener("mousemove",move,false);element.ownerDocument.removeEventListener("mouseup",stop,false);if(onstop)
onstop(element,(e));}
dragging=false;}
element.addEventListener("mousedown",start,false);};WebInspector.Spectrum.prototype={setColor:function(color)
{this._hsv=color.hsva();},color:function()
{return WebInspector.Color.fromHSVA(this._hsv);},_colorString:function()
{var cf=WebInspector.Color.Format;var format=this._originalFormat;var color=this.color();var originalFormatString=color.asString(this._originalFormat);if(originalFormatString)
return originalFormatString;if(color.hasAlpha()){if(format===cf.HSLA||format===cf.HSL)
return color.asString(cf.HSLA);else
return color.asString(cf.RGBA);}
if(format===cf.ShortHEX)
return color.asString(cf.HEX);console.assert(format===cf.Nickname);return color.asString(cf.RGB);},set displayText(text)
{this._displayElement.textContent=text;},_onchange:function()
{this._updateUI();this.dispatchEventToListeners(WebInspector.Spectrum.Events.ColorChanged,this._colorString());},_updateHelperLocations:function()
{var h=this._hsv[0];var s=this._hsv[1];var v=this._hsv[2];var dragX=s*this.dragWidth;var dragY=this.dragHeight-(v*this.dragHeight);dragX=Math.max(-this._dragHelperElementHeight,Math.min(this.dragWidth-this._dragHelperElementHeight,dragX-this._dragHelperElementHeight));dragY=Math.max(-this._dragHelperElementHeight,Math.min(this.dragHeight-this._dragHelperElementHeight,dragY-this._dragHelperElementHeight));this._dragHelperElement.positionAt(dragX,dragY);var slideY=this.slideHeight-((h*this.slideHeight)+this.slideHelperHeight);this.slideHelper.style.top=slideY+"px";this._alphaElement.value=this._hsv[3]*100;},_updateUI:function()
{this._updateHelperLocations();this._draggerElement.style.backgroundColor=(WebInspector.Color.fromHSVA([this._hsv[0],1,1,1]).asString(WebInspector.Color.Format.RGB));this._swatchInnerElement.style.backgroundColor=(this.color().asString(WebInspector.Color.Format.RGBA));this._alphaElement.value=this._hsv[3]*100;},wasShown:function()
{this.slideHeight=this._sliderElement.offsetHeight;this.dragWidth=this._draggerElement.offsetWidth;this.dragHeight=this._draggerElement.offsetHeight;this._dragHelperElementHeight=this._dragHelperElement.offsetHeight/2;this.slideHelperHeight=this.slideHelper.offsetHeight/2;this._updateUI();},__proto__:WebInspector.VBox.prototype}
WebInspector.SpectrumPopupHelper=function()
{this._spectrum=new WebInspector.Spectrum();this._spectrum.contentElement.addEventListener("keydown",this._onKeyDown.bind(this),false);this._popover=new WebInspector.Popover();this._popover.setCanShrink(false);this._popover.element.addEventListener("mousedown",consumeEvent,false);this._hideProxy=this.hide.bind(this,true);}
WebInspector.SpectrumPopupHelper.Events={Hidden:"Hidden"};WebInspector.SpectrumPopupHelper.prototype={spectrum:function()
{return this._spectrum;},toggle:function(element,color,format)
{if(this._popover.isShowing())
this.hide(true);else
this.show(element,color,format);return this._popover.isShowing();},show:function(element,color,format)
{if(this._popover.isShowing()){if(this._anchorElement===element)
return false;this.hide(true);}
delete this._isHidden;this._anchorElement=element;this._spectrum.setColor(color);this._spectrum._originalFormat=format!==WebInspector.Color.Format.Original?format:color.format();this.reposition(element);var document=this._popover.element.ownerDocument;document.addEventListener("mousedown",this._hideProxy,false);document.defaultView.addEventListener("resize",this._hideProxy,false);WebInspector.targetManager.addModelListener(WebInspector.ResourceTreeModel,WebInspector.ResourceTreeModel.EventTypes.ColorPicked,this._colorPicked,this);PageAgent.setColorPickerEnabled(true);return true;},reposition:function(element)
{if(!this._previousFocusElement)
this._previousFocusElement=WebInspector.currentFocusElement();this._popover.showView(this._spectrum,element);WebInspector.setCurrentFocusElement(this._spectrum.contentElement);},hide:function(commitEdit)
{if(this._isHidden)
return;var document=this._popover.element.ownerDocument;this._isHidden=true;this._popover.hide();document.removeEventListener("mousedown",this._hideProxy,false);document.defaultView.removeEventListener("resize",this._hideProxy,false);PageAgent.setColorPickerEnabled(false);WebInspector.targetManager.removeModelListener(WebInspector.ResourceTreeModel,WebInspector.ResourceTreeModel.EventTypes.ColorPicked,this._colorPicked,this);this.dispatchEventToListeners(WebInspector.SpectrumPopupHelper.Events.Hidden,!!commitEdit);WebInspector.setCurrentFocusElement(this._previousFocusElement);delete this._previousFocusElement;delete this._anchorElement;},_onKeyDown:function(event)
{if(event.keyIdentifier==="Enter"){this.hide(true);event.consume(true);return;}
if(event.keyIdentifier==="U+001B"){this.hide(false);event.consume(true);}},_colorPicked:function(event)
{var color=(event.data);var rgba=[color.r,color.g,color.b,(color.a/2.55|0)/100];this._spectrum.setColor(WebInspector.Color.fromRGBA(rgba));this._spectrum._onchange();InspectorFrontendHost.bringToFront();},__proto__:WebInspector.Object.prototype}
WebInspector.ColorSwatch=function(readOnly)
{this.element=createElementWithClass("span","swatch");this._swatchInnerElement=this.element.createChild("span","swatch-inner");var shiftClickMessage=WebInspector.UIString("Shift-click to change color format.");this.element.title=readOnly?shiftClickMessage:String.sprintf("%s\n%s",WebInspector.UIString("Click to open a colorpicker."),shiftClickMessage);this.element.addEventListener("mousedown",consumeEvent,false);this.element.addEventListener("dblclick",consumeEvent,false);}
WebInspector.ColorSwatch.prototype={setColorString:function(colorString)
{this._swatchInnerElement.style.backgroundColor=colorString;}};WebInspector.ElementsBreadcrumbs=function()
{WebInspector.HBox.call(this,true);this.registerRequiredCSS("elements/breadcrumbs.css");this.crumbsElement=this.contentElement.createChild("div","crumbs");this.crumbsElement.addEventListener("mousemove",this._mouseMovedInCrumbs.bind(this),false);this.crumbsElement.addEventListener("mouseleave",this._mouseMovedOutOfCrumbs.bind(this),false);}
WebInspector.ElementsBreadcrumbs.Events={NodeSelected:"NodeSelected"}
WebInspector.ElementsBreadcrumbs.prototype={wasShown:function()
{this.update();},updateNodes:function(nodes)
{if(!nodes.length)
return;var crumbs=this.crumbsElement;for(var crumb=crumbs.firstChild;crumb;crumb=crumb.nextSibling){if(nodes.indexOf(crumb.representedObject)!==-1){this.update(true);return;}}},setSelectedNode:function(node)
{this._currentDOMNode=node;this.update();},_mouseMovedInCrumbs:function(event)
{var nodeUnderMouse=event.target;var crumbElement=nodeUnderMouse.enclosingNodeOrSelfWithClass("crumb");var node=(crumbElement?crumbElement.representedObject:null);if(node)
node.highlight();},_mouseMovedOutOfCrumbs:function(event)
{if(this._currentDOMNode)
this._currentDOMNode.domModel().hideDOMNodeHighlight();},update:function(force)
{if(!this.isShowing())
return;var currentDOMNode=this._currentDOMNode;var crumbs=this.crumbsElement;var handled=false;var crumb=crumbs.firstChild;while(crumb){if(crumb.representedObject===currentDOMNode){crumb.classList.add("selected");handled=true;}else{crumb.classList.remove("selected");}
crumb=crumb.nextSibling;}
if(handled&&!force){this.updateSizes();return;}
crumbs.removeChildren();var panel=this;function selectCrumb(event)
{event.preventDefault();var crumb=(event.currentTarget);if(!crumb.classList.contains("collapsed")){this.dispatchEventToListeners(WebInspector.ElementsBreadcrumbs.Events.NodeSelected,crumb.representedObject);return;}
if(crumb===panel.crumbsElement.firstChild){var currentCrumb=crumb;while(currentCrumb){var hidden=currentCrumb.classList.contains("hidden");var collapsed=currentCrumb.classList.contains("collapsed");if(!hidden&&!collapsed)
break;crumb=currentCrumb;currentCrumb=currentCrumb.nextSiblingElement;}}
this.updateSizes(crumb);}
var boundSelectCrumb=selectCrumb.bind(this);for(var current=currentDOMNode;current;current=current.parentNode){if(current.nodeType()===Node.DOCUMENT_NODE)
continue;crumb=createElementWithClass("span","crumb");crumb.representedObject=current;crumb.addEventListener("mousedown",boundSelectCrumb,false);var crumbTitle="";switch(current.nodeType()){case Node.ELEMENT_NODE:if(current.pseudoType())
crumbTitle="::"+current.pseudoType();else
WebInspector.DOMPresentationUtils.decorateNodeLabel(current,crumb);break;case Node.TEXT_NODE:crumbTitle=WebInspector.UIString("(text)");break;case Node.COMMENT_NODE:crumbTitle="<!-->";break;case Node.DOCUMENT_TYPE_NODE:crumbTitle="<!DOCTYPE>";break;case Node.DOCUMENT_FRAGMENT_NODE:crumbTitle=current.shadowRootType()?"#shadow-root":current.nodeNameInCorrectCase();break;default:crumbTitle=current.nodeNameInCorrectCase();}
if(!crumb.childNodes.length){var nameElement=createElement("span");nameElement.textContent=crumbTitle;crumb.appendChild(nameElement);crumb.title=crumbTitle;}
if(current===currentDOMNode)
crumb.classList.add("selected");crumbs.insertBefore(crumb,crumbs.firstChild);}
this.updateSizes();},updateSizes:function(focusedCrumb)
{if(!this.isShowing())
return;var crumbs=this.crumbsElement;if(!crumbs.firstChild)
return;var selectedIndex=0;var focusedIndex=0;var selectedCrumb;for(var i=0;i<crumbs.childNodes.length;++i){var crumb=crumbs.childNodes[i];if(!selectedCrumb&&crumb.classList.contains("selected")){selectedCrumb=crumb;selectedIndex=i;}
if(crumb===focusedCrumb)
focusedIndex=i;crumb.classList.remove("compact","collapsed","hidden");}
var contentElementWidth=this.contentElement.offsetWidth;var normalSizes=[];for(var i=0;i<crumbs.childNodes.length;++i){var crumb=crumbs.childNodes[i];normalSizes[i]=crumb.offsetWidth;}
var compactSizes=[];for(var i=0;i<crumbs.childNodes.length;++i){var crumb=crumbs.childNodes[i];crumb.classList.add("compact");}
for(var i=0;i<crumbs.childNodes.length;++i){var crumb=crumbs.childNodes[i];compactSizes[i]=crumb.offsetWidth;}
crumbs.firstChild.classList.add("collapsed");var collapsedSize=crumbs.firstChild.offsetWidth;for(var i=0;i<crumbs.childNodes.length;++i){var crumb=crumbs.childNodes[i];crumb.classList.remove("compact","collapsed");}
function crumbsAreSmallerThanContainer()
{var totalSize=0;for(var i=0;i<crumbs.childNodes.length;++i){var crumb=crumbs.childNodes[i];if(crumb.classList.contains("hidden"))
continue;if(crumb.classList.contains("collapsed")){totalSize+=collapsedSize;continue;}
totalSize+=crumb.classList.contains("compact")?compactSizes[i]:normalSizes[i];}
const rightPadding=10;return totalSize+rightPadding<contentElementWidth;}
if(crumbsAreSmallerThanContainer())
return;var BothSides=0;var AncestorSide=-1;var ChildSide=1;function makeCrumbsSmaller(shrinkingFunction,direction)
{var significantCrumb=focusedCrumb||selectedCrumb;var significantIndex=significantCrumb===selectedCrumb?selectedIndex:focusedIndex;function shrinkCrumbAtIndex(index)
{var shrinkCrumb=crumbs.childNodes[index];if(shrinkCrumb&&shrinkCrumb!==significantCrumb)
shrinkingFunction(shrinkCrumb);if(crumbsAreSmallerThanContainer())
return true;return false;}
if(direction){var index=(direction>0?0:crumbs.childNodes.length-1);while(index!==significantIndex){if(shrinkCrumbAtIndex(index))
return true;index+=(direction>0?1:-1);}}else{var startIndex=0;var endIndex=crumbs.childNodes.length-1;while(startIndex!=significantIndex||endIndex!=significantIndex){var startDistance=significantIndex-startIndex;var endDistance=endIndex-significantIndex;if(startDistance>=endDistance)
var index=startIndex++;else
var index=endIndex--;if(shrinkCrumbAtIndex(index))
return true;}}
return false;}
function coalesceCollapsedCrumbs()
{var crumb=crumbs.firstChild;var collapsedRun=false;var newStartNeeded=false;var newEndNeeded=false;while(crumb){var hidden=crumb.classList.contains("hidden");if(!hidden){var collapsed=crumb.classList.contains("collapsed");if(collapsedRun&&collapsed){crumb.classList.add("hidden");crumb.classList.remove("compact");crumb.classList.remove("collapsed");if(crumb.classList.contains("start")){crumb.classList.remove("start");newStartNeeded=true;}
if(crumb.classList.contains("end")){crumb.classList.remove("end");newEndNeeded=true;}
continue;}
collapsedRun=collapsed;if(newEndNeeded){newEndNeeded=false;crumb.classList.add("end");}}else{collapsedRun=true;}
crumb=crumb.nextSibling;}
if(newStartNeeded){crumb=crumbs.lastChild;while(crumb){if(!crumb.classList.contains("hidden")){crumb.classList.add("start");break;}
crumb=crumb.previousSibling;}}}
function compact(crumb)
{if(crumb.classList.contains("hidden"))
return;crumb.classList.add("compact");}
function collapse(crumb,dontCoalesce)
{if(crumb.classList.contains("hidden"))
return;crumb.classList.add("collapsed");crumb.classList.remove("compact");if(!dontCoalesce)
coalesceCollapsedCrumbs();}
if(!focusedCrumb){if(makeCrumbsSmaller(compact,ChildSide))
return;if(makeCrumbsSmaller(collapse,ChildSide))
return;}
if(makeCrumbsSmaller(compact,focusedCrumb?BothSides:AncestorSide))
return;if(makeCrumbsSmaller(collapse,focusedCrumb?BothSides:AncestorSide))
return;if(!selectedCrumb)
return;compact(selectedCrumb);if(crumbsAreSmallerThanContainer())
return;collapse(selectedCrumb,true);},__proto__:WebInspector.HBox.prototype};WebInspector.ElementsSidebarPane=function(title)
{WebInspector.SidebarPane.call(this,title);this._updateThrottler=new WebInspector.Throttler(100);this._node=null;}
WebInspector.ElementsSidebarPane.prototype={node:function()
{return this._node;},setNode:function(node)
{this._node=node;this.update();},doUpdate:function(finishedCallback)
{finishedCallback();},update:function()
{this._updateWhenVisible=!this.isShowing();if(this._updateWhenVisible)
return;this._updateThrottler.schedule(innerUpdate.bind(this));function innerUpdate(finishedCallback)
{if(this.isShowing())
this.doUpdate(finishedCallback);else
finishedCallback();}},wasShown:function()
{WebInspector.SidebarPane.prototype.wasShown.call(this);this.update();},__proto__:WebInspector.SidebarPane.prototype};WebInspector.ElementsTreeElement=function(node,elementCloseTag)
{TreeElement.call(this,"",node);this._node=node;this._elementCloseTag=elementCloseTag;if(this._node.nodeType()==Node.ELEMENT_NODE&&!elementCloseTag)
this._canAddAttributes=true;this._searchQuery=null;this._expandedChildrenLimit=WebInspector.ElementsTreeElement.InitialChildrenLimit;}
WebInspector.ElementsTreeElement.InitialChildrenLimit=500;WebInspector.ElementsTreeElement.ForbiddenClosingTagElements=["area","base","basefont","br","canvas","col","command","embed","frame","hr","img","input","keygen","link","menuitem","meta","param","source","track","wbr"].keySet();WebInspector.ElementsTreeElement.EditTagBlacklist=["html","head","body"].keySet();WebInspector.ElementsTreeElement.ChildrenDisplayMode={NoChildren:0,InlineText:1,HasChildren:2}
WebInspector.ElementsTreeElement.animateOnDOMUpdate=function(treeElement)
{var tagName=treeElement.listItemElement.querySelector(".webkit-html-tag-name");WebInspector.runCSSAnimationOnce(tagName||treeElement.listItemElement,"dom-update-highlight");}
WebInspector.ElementsTreeElement.prototype={isClosingTag:function()
{return!!this._elementCloseTag;},node:function()
{return this._node;},isEditing:function()
{return!!this._editing;},highlightSearchResults:function(searchQuery)
{if(this._searchQuery!==searchQuery)
this._hideSearchHighlight();this._searchQuery=searchQuery;this._searchHighlightsVisible=true;this.updateTitle(true);},hideSearchHighlights:function()
{delete this._searchHighlightsVisible;this._hideSearchHighlight();},_hideSearchHighlight:function()
{if(!this._highlightResult)
return;function updateEntryHide(entry)
{switch(entry.type){case"added":entry.node.remove();break;case"changed":entry.node.textContent=entry.oldText;break;}}
for(var i=(this._highlightResult.length-1);i>=0;--i)
updateEntryHide(this._highlightResult[i]);delete this._highlightResult;},setInClipboard:function(inClipboard)
{if(this._inClipboard===inClipboard)
return;this._inClipboard=inClipboard;this.listItemElement.classList.toggle("in-clipboard",inClipboard);},get hovered()
{return this._hovered;},set hovered(x)
{if(this._hovered===x)
return;this._hovered=x;if(this.listItemElement){if(x){this.updateSelection();this.listItemElement.classList.add("hovered");}else{this.listItemElement.classList.remove("hovered");}}},expandedChildrenLimit:function()
{return this._expandedChildrenLimit;},setExpandedChildrenLimit:function(expandedChildrenLimit)
{this._expandedChildrenLimit=expandedChildrenLimit;},childrenDisplayMode:function()
{return this._childrenDisplayMode;},setChildrenDisplayMode:function(displayMode)
{this._childrenDisplayMode=displayMode;},updateSelection:function()
{var listItemElement=this.listItemElement;if(!listItemElement)
return;if(!this._readyToUpdateSelection){if(listItemElement.ownerDocument.body.offsetWidth>0)
this._readyToUpdateSelection=true;else{return;}}
if(!this.selectionElement){this.selectionElement=createElement("div");this.selectionElement.className="selection selected";listItemElement.insertBefore(this.selectionElement,listItemElement.firstChild);}
this.selectionElement.style.height=listItemElement.offsetHeight+"px";},onattach:function()
{if(this._hovered){this.updateSelection();this.listItemElement.classList.add("hovered");}
this.updateTitle();this._preventFollowingLinksOnDoubleClick();this.listItemElement.draggable=true;},_preventFollowingLinksOnDoubleClick:function()
{var links=this.listItemElement.querySelectorAll("li .webkit-html-tag > .webkit-html-attribute > .webkit-html-external-link, li .webkit-html-tag > .webkit-html-attribute > .webkit-html-resource-link");if(!links)
return;for(var i=0;i<links.length;++i)
links[i].preventFollowOnDoubleClick=true;},onpopulate:function()
{this.populated=true;this.treeOutline.populateTreeElement(this);},setUpdateInfo:function(updateInfo)
{this._updateInfo=updateInfo;},expandRecursively:function()
{function callback()
{TreeElement.prototype.expandRecursively.call(this,Number.MAX_VALUE);}
this._node.getSubtree(-1,callback.bind(this));},onexpand:function()
{if(this._elementCloseTag)
return;this.updateTitle();this.treeOutline.updateSelection();},oncollapse:function()
{if(this._elementCloseTag)
return;this.updateTitle();this.treeOutline.updateSelection();},onreveal:function()
{if(this.listItemElement){var tagSpans=this.listItemElement.getElementsByClassName("webkit-html-tag-name");if(tagSpans.length)
tagSpans[0].scrollIntoViewIfNeeded(true);else
this.listItemElement.scrollIntoViewIfNeeded(true);}},select:function(omitFocus,selectedByUser)
{if(this._editing)
return false;if(this.treeOutline.handlePickNode(this.title,this._node))
return true;return TreeElement.prototype.select.call(this,omitFocus,selectedByUser);},onselect:function(selectedByUser)
{this.treeOutline.suppressRevealAndSelect=true;this.treeOutline.selectDOMNode(this._node,selectedByUser);if(selectedByUser)
this._node.highlight();this.updateSelection();this.treeOutline.suppressRevealAndSelect=false;return true;},ondelete:function()
{var startTagTreeElement=this.treeOutline.findTreeElement(this._node);startTagTreeElement?startTagTreeElement.remove():this.remove();return true;},onenter:function()
{if(this._editing)
return false;this._startEditing();return true;},selectOnMouseDown:function(event)
{TreeElement.prototype.selectOnMouseDown.call(this,event);if(this._editing)
return;if(event.detail>=2)
event.preventDefault();},ondblclick:function(event)
{if(this._editing||this._elementCloseTag)
return false;if(this._startEditingTarget((event.target)))
return false;if(this.hasChildren&&!this.expanded)
this.expand();return false;},hasEditableNode:function()
{return!this._node.isShadowRoot()&&!this._node.ancestorUserAgentShadowRoot();},_insertInLastAttributePosition:function(tag,node)
{if(tag.getElementsByClassName("webkit-html-attribute").length>0)
tag.insertBefore(node,tag.lastChild);else{var nodeName=tag.textContent.match(/^<(.*?)>$/)[1];tag.textContent='';tag.createTextChild('<'+nodeName);tag.appendChild(node);tag.createTextChild('>');}
this.updateSelection();},_startEditingTarget:function(eventTarget)
{if(this.treeOutline.selectedDOMNode()!=this._node)
return false;if(this._node.nodeType()!=Node.ELEMENT_NODE&&this._node.nodeType()!=Node.TEXT_NODE)
return false;if(this.treeOutline.pickNodeMode())
return false;var textNode=eventTarget.enclosingNodeOrSelfWithClass("webkit-html-text-node");if(textNode)
return this._startEditingTextNode(textNode);var attribute=eventTarget.enclosingNodeOrSelfWithClass("webkit-html-attribute");if(attribute)
return this._startEditingAttribute(attribute,eventTarget);var tagName=eventTarget.enclosingNodeOrSelfWithClass("webkit-html-tag-name");if(tagName)
return this._startEditingTagName(tagName);var newAttribute=eventTarget.enclosingNodeOrSelfWithClass("add-attribute");if(newAttribute)
return this._addNewAttribute();return false;},populateTagContextMenu:function(contextMenu,event)
{var treeElement=this._elementCloseTag?this.treeOutline.findTreeElement(this._node):this;contextMenu.appendItem(WebInspector.UIString.capitalize("Add ^attribute"),treeElement._addNewAttribute.bind(treeElement));var attribute=event.target.enclosingNodeOrSelfWithClass("webkit-html-attribute");var newAttribute=event.target.enclosingNodeOrSelfWithClass("add-attribute");if(attribute&&!newAttribute)
contextMenu.appendItem(WebInspector.UIString.capitalize("Edit ^attribute"),this._startEditingAttribute.bind(this,attribute,event.target));contextMenu.appendSeparator();if(this.treeOutline.setPseudoClassCallback){var pseudoSubMenu=contextMenu.appendSubMenuItem(WebInspector.UIString.capitalize("Force ^element ^state"));this._populateForcedPseudoStateItems(pseudoSubMenu);contextMenu.appendSeparator();}
this.populateNodeContextMenu(contextMenu);this.populateScrollIntoView(contextMenu);},populateScrollIntoView:function(contextMenu)
{contextMenu.appendSeparator();contextMenu.appendItem(WebInspector.UIString.capitalize("Scroll into ^view"),this._scrollIntoView.bind(this));},_populateForcedPseudoStateItems:function(subMenu)
{const pseudoClasses=["active","hover","focus","visited"];var node=this._node;var forcedPseudoState=(node?node.getUserProperty("pseudoState"):null)||[];for(var i=0;i<pseudoClasses.length;++i){var pseudoClassForced=forcedPseudoState.indexOf(pseudoClasses[i])>=0;var setPseudoClassCallback=this.treeOutline.setPseudoClassCallback.bind(this.treeOutline,node,pseudoClasses[i],!pseudoClassForced);subMenu.appendCheckboxItem(":"+pseudoClasses[i],setPseudoClassCallback,pseudoClassForced,false);}},populateTextContextMenu:function(contextMenu,textNode)
{if(!this._editing)
contextMenu.appendItem(WebInspector.UIString.capitalize("Edit ^text"),this._startEditingTextNode.bind(this,textNode));this.populateNodeContextMenu(contextMenu);},populateNodeContextMenu:function(contextMenu)
{var openTagElement=this.treeOutline.getCachedTreeElement(this._node)||this;var isEditable=this.hasEditableNode();if(isEditable&&!this._editing)
contextMenu.appendItem(WebInspector.UIString("Edit as HTML"),openTagElement.toggleEditAsHTML.bind(openTagElement));var isShadowRoot=this._node.isShadowRoot();if(this._node.nodeType()===Node.ELEMENT_NODE)
contextMenu.appendItem(WebInspector.UIString.capitalize("Copy CSS ^path"),this._copyCSSPath.bind(this));if(!isShadowRoot)
contextMenu.appendItem(WebInspector.UIString("Copy XPath"),this._copyXPath.bind(this));if(!isShadowRoot){var treeOutline=this.treeOutline;contextMenu.appendSeparator();contextMenu.appendItem(WebInspector.UIString("Cut"),treeOutline.performCopyOrCut.bind(treeOutline,true,this._node),!this.hasEditableNode());contextMenu.appendItem(WebInspector.UIString("Copy"),treeOutline.performCopyOrCut.bind(treeOutline,false,this._node));contextMenu.appendItem(WebInspector.UIString("Paste"),treeOutline.pasteNode.bind(treeOutline,this._node),!treeOutline.canPaste(this._node));}
if(isEditable)
contextMenu.appendItem(WebInspector.UIString("Delete"),this.remove.bind(this));contextMenu.appendSeparator();},_startEditing:function()
{if(this.treeOutline.selectedDOMNode()!==this._node)
return;var listItem=this._listItemNode;if(this._canAddAttributes){var attribute=listItem.getElementsByClassName("webkit-html-attribute")[0];if(attribute)
return this._startEditingAttribute(attribute,attribute.getElementsByClassName("webkit-html-attribute-value")[0]);return this._addNewAttribute();}
if(this._node.nodeType()===Node.TEXT_NODE){var textNode=listItem.getElementsByClassName("webkit-html-text-node")[0];if(textNode)
return this._startEditingTextNode(textNode);return;}},_addNewAttribute:function()
{var container=createElement("span");this._buildAttributeDOM(container," ","");var attr=container.firstElementChild;attr.style.marginLeft="2px";attr.style.marginRight="2px";var tag=this.listItemElement.getElementsByClassName("webkit-html-tag")[0];this._insertInLastAttributePosition(tag,attr);attr.scrollIntoViewIfNeeded(true);return this._startEditingAttribute(attr,attr);},_triggerEditAttribute:function(attributeName)
{var attributeElements=this.listItemElement.getElementsByClassName("webkit-html-attribute-name");for(var i=0,len=attributeElements.length;i<len;++i){if(attributeElements[i].textContent===attributeName){for(var elem=attributeElements[i].nextSibling;elem;elem=elem.nextSibling){if(elem.nodeType!==Node.ELEMENT_NODE)
continue;if(elem.classList.contains("webkit-html-attribute-value"))
return this._startEditingAttribute(elem.parentNode,elem);}}}},_startEditingAttribute:function(attribute,elementForSelection)
{console.assert(this.listItemElement.isAncestor(attribute));if(WebInspector.isBeingEdited(attribute))
return true;var attributeNameElement=attribute.getElementsByClassName("webkit-html-attribute-name")[0];if(!attributeNameElement)
return false;var attributeName=attributeNameElement.textContent;var attributeValueElement=attribute.getElementsByClassName("webkit-html-attribute-value")[0];elementForSelection=attributeValueElement.isAncestor(elementForSelection)?attributeValueElement:elementForSelection;function removeZeroWidthSpaceRecursive(node)
{if(node.nodeType===Node.TEXT_NODE){node.nodeValue=node.nodeValue.replace(/\u200B/g,"");return;}
if(node.nodeType!==Node.ELEMENT_NODE)
return;for(var child=node.firstChild;child;child=child.nextSibling)
removeZeroWidthSpaceRecursive(child);}
var attributeValue=attributeName&&attributeValueElement?this._node.getAttribute(attributeName):undefined;if(attributeValue!==undefined)
attributeValueElement.textContent=attributeValue;removeZeroWidthSpaceRecursive(attribute);var config=new WebInspector.InplaceEditor.Config(this._attributeEditingCommitted.bind(this),this._editingCancelled.bind(this),attributeName);function handleKeyDownEvents(event)
{var isMetaOrCtrl=WebInspector.isMac()?event.metaKey&&!event.shiftKey&&!event.ctrlKey&&!event.altKey:event.ctrlKey&&!event.shiftKey&&!event.metaKey&&!event.altKey;if(isEnterKey(event)&&(event.isMetaOrCtrlForTest||!config.multiline||isMetaOrCtrl))
return"commit";else if(event.keyCode===WebInspector.KeyboardShortcut.Keys.Esc.code||event.keyIdentifier==="U+001B")
return"cancel";else if(event.keyIdentifier==="U+0009")
return"move-"+(event.shiftKey?"backward":"forward");else{WebInspector.handleElementValueModifications(event,attribute);return"";}}
config.customFinishHandler=handleKeyDownEvents;this._editing=WebInspector.InplaceEditor.startEditing(attribute,config);this.listItemElement.window().getSelection().setBaseAndExtent(elementForSelection,0,elementForSelection,1);return true;},_startEditingTextNode:function(textNodeElement)
{if(WebInspector.isBeingEdited(textNodeElement))
return true;var textNode=this._node;if(textNode.nodeType()===Node.ELEMENT_NODE&&textNode.firstChild)
textNode=textNode.firstChild;var container=textNodeElement.enclosingNodeOrSelfWithClass("webkit-html-text-node");if(container)
container.textContent=textNode.nodeValue();var config=new WebInspector.InplaceEditor.Config(this._textNodeEditingCommitted.bind(this,textNode),this._editingCancelled.bind(this));this._editing=WebInspector.InplaceEditor.startEditing(textNodeElement,config);this.listItemElement.window().getSelection().setBaseAndExtent(textNodeElement,0,textNodeElement,1);return true;},_startEditingTagName:function(tagNameElement)
{if(!tagNameElement){tagNameElement=this.listItemElement.getElementsByClassName("webkit-html-tag-name")[0];if(!tagNameElement)
return false;}
var tagName=tagNameElement.textContent;if(WebInspector.ElementsTreeElement.EditTagBlacklist[tagName.toLowerCase()])
return false;if(WebInspector.isBeingEdited(tagNameElement))
return true;var closingTagElement=this._distinctClosingTagElement();function keyupListener(event)
{if(closingTagElement)
closingTagElement.textContent="</"+tagNameElement.textContent+">";}
function editingComitted(element,newTagName)
{tagNameElement.removeEventListener('keyup',keyupListener,false);this._tagNameEditingCommitted.apply(this,arguments);}
function editingCancelled()
{tagNameElement.removeEventListener('keyup',keyupListener,false);this._editingCancelled.apply(this,arguments);}
tagNameElement.addEventListener('keyup',keyupListener,false);var config=new WebInspector.InplaceEditor.Config(editingComitted.bind(this),editingCancelled.bind(this),tagName);this._editing=WebInspector.InplaceEditor.startEditing(tagNameElement,config);this.listItemElement.window().getSelection().setBaseAndExtent(tagNameElement,0,tagNameElement,1);return true;},_startEditingAsHTML:function(commitCallback,error,initialValue)
{if(error)
return;if(this._editing)
return;function consume(event)
{if(event.eventPhase===Event.AT_TARGET)
event.consume(true);}
initialValue=this._convertWhitespaceToEntities(initialValue).text;this._htmlEditElement=createElement("div");this._htmlEditElement.className="source-code elements-tree-editor";var child=this.listItemElement.firstChild;while(child){child.style.display="none";child=child.nextSibling;}
if(this._childrenListNode)
this._childrenListNode.style.display="none";this.listItemElement.appendChild(this._htmlEditElement);this.treeOutline.childrenListElement.parentElement.addEventListener("mousedown",consume,false);this.updateSelection();function commit(element,newValue)
{commitCallback(initialValue,newValue);dispose.call(this);}
function dispose()
{delete this._editing;this.treeOutline.setMultilineEditing(null);this.listItemElement.removeChild(this._htmlEditElement);delete this._htmlEditElement;if(this._childrenListNode)
this._childrenListNode.style.removeProperty("display");var child=this.listItemElement.firstChild;while(child){child.style.removeProperty("display");child=child.nextSibling;}
this.treeOutline.childrenListElement.parentElement.removeEventListener("mousedown",consume,false);this.updateSelection();this.treeOutline.focus();}
var config=new WebInspector.InplaceEditor.Config(commit.bind(this),dispose.bind(this));config.setMultilineOptions(initialValue,{name:"xml",htmlMode:true},"web-inspector-html",WebInspector.settings.domWordWrap.get(),true);WebInspector.InplaceEditor.startMultilineEditing(this._htmlEditElement,config).then(markAsBeingEdited.bind(this));function markAsBeingEdited(controller)
{this._editing=(controller);this._editing.setWidth(this.treeOutline.visibleWidth());this.treeOutline.setMultilineEditing(this._editing);}},_attributeEditingCommitted:function(element,newText,oldText,attributeName,moveDirection)
{delete this._editing;var treeOutline=this.treeOutline;function moveToNextAttributeIfNeeded(error)
{if(error)
this._editingCancelled(element,attributeName);if(!moveDirection)
return;treeOutline.runPendingUpdates();var attributes=this._node.attributes();for(var i=0;i<attributes.length;++i){if(attributes[i].name!==attributeName)
continue;if(moveDirection==="backward"){if(i===0)
this._startEditingTagName();else
this._triggerEditAttribute(attributes[i-1].name);}else{if(i===attributes.length-1)
this._addNewAttribute();else
this._triggerEditAttribute(attributes[i+1].name);}
return;}
if(moveDirection==="backward"){if(newText===" "){if(attributes.length>0)
this._triggerEditAttribute(attributes[attributes.length-1].name);}else{if(attributes.length>1)
this._triggerEditAttribute(attributes[attributes.length-2].name);}}else if(moveDirection==="forward"){if(!/^\s*$/.test(newText))
this._addNewAttribute();else
this._startEditingTagName();}}
if((attributeName.trim()||newText.trim())&&oldText!==newText){this._node.setAttribute(attributeName,newText,moveToNextAttributeIfNeeded.bind(this));return;}
this.updateTitle();moveToNextAttributeIfNeeded.call(this);},_tagNameEditingCommitted:function(element,newText,oldText,tagName,moveDirection)
{delete this._editing;var self=this;function cancel()
{var closingTagElement=self._distinctClosingTagElement();if(closingTagElement)
closingTagElement.textContent="</"+tagName+">";self._editingCancelled(element,tagName);moveToNextAttributeIfNeeded.call(self);}
function moveToNextAttributeIfNeeded()
{if(moveDirection!=="forward"){this._addNewAttribute();return;}
var attributes=this._node.attributes();if(attributes.length>0)
this._triggerEditAttribute(attributes[0].name);else
this._addNewAttribute();}
newText=newText.trim();if(newText===oldText){cancel();return;}
var treeOutline=this.treeOutline;var wasExpanded=this.expanded;function changeTagNameCallback(error,nodeId)
{if(error||!nodeId){cancel();return;}
var newTreeItem=treeOutline.selectNodeAfterEdit(wasExpanded,error,nodeId);moveToNextAttributeIfNeeded.call(newTreeItem);}
this._node.setNodeName(newText,changeTagNameCallback);},_textNodeEditingCommitted:function(textNode,element,newText)
{delete this._editing;function callback()
{this.updateTitle();}
textNode.setNodeValue(newText,callback.bind(this));},_editingCancelled:function(element,context)
{delete this._editing;this.updateTitle();},_distinctClosingTagElement:function()
{if(this.expanded){var closers=this._childrenListNode.querySelectorAll(".close");return closers[closers.length-1];}
var tags=this.listItemElement.getElementsByClassName("webkit-html-tag");return(tags.length===1?null:tags[tags.length-1]);},updateTitle:function(onlySearchQueryChanged)
{if(this._editing)
return;if(onlySearchQueryChanged){this._hideSearchHighlight();}else{var nodeInfo=this._nodeTitleInfo(WebInspector.linkifyURLAsNode);if(nodeInfo.shadowRoot)
this.listItemElement.classList.add("shadow-root");var highlightElement=createElement("span");highlightElement.className="highlight";highlightElement.appendChild(nodeInfo.titleDOM);this.title=highlightElement;this._updateDecorations();delete this._highlightResult;}
delete this.selectionElement;if(this.selected)
this.updateSelection();this._preventFollowingLinksOnDoubleClick();this._highlightSearchResults();},_createDecoratorElement:function()
{var node=this._node;var decoratorMessages=[];var parentDecoratorMessages=[];var decorators=this.treeOutline.nodeDecorators();for(var i=0;i<decorators.length;++i){var decorator=decorators[i];var message=decorator.decorate(node);if(message){decoratorMessages.push(message);continue;}
if(this.expanded||this._elementCloseTag)
continue;message=decorator.decorateAncestor(node);if(message)
parentDecoratorMessages.push(message)}
if(!decoratorMessages.length&&!parentDecoratorMessages.length)
return null;var decoratorElement=createElement("div");decoratorElement.classList.add("elements-gutter-decoration");if(!decoratorMessages.length)
decoratorElement.classList.add("elements-has-decorated-children");decoratorElement.title=decoratorMessages.concat(parentDecoratorMessages).join("\n");return decoratorElement;},_updateDecorations:function()
{if(this._decoratorElement)
this._decoratorElement.remove();this._decoratorElement=this._createDecoratorElement();if(this._decoratorElement&&this.listItemElement)
this.listItemElement.insertBefore(this._decoratorElement,this.listItemElement.firstChild);},_buildAttributeDOM:function(parentElement,name,value,forceValue,node,linkify)
{var closingPunctuationRegex=/[\/;:\)\]\}]/g;var highlightIndex=0;var highlightCount;var additionalHighlightOffset=0;var result;function replacer(match,replaceOffset){while(highlightIndex<highlightCount&&result.entityRanges[highlightIndex].offset<replaceOffset){result.entityRanges[highlightIndex].offset+=additionalHighlightOffset;++highlightIndex;}
additionalHighlightOffset+=1;return match+"\u200B";}
function setValueWithEntities(element,value)
{result=this._convertWhitespaceToEntities(value);highlightCount=result.entityRanges.length;value=result.text.replace(closingPunctuationRegex,replacer);while(highlightIndex<highlightCount){result.entityRanges[highlightIndex].offset+=additionalHighlightOffset;++highlightIndex;}
element.textContent=value;WebInspector.highlightRangesWithStyleClass(element,result.entityRanges,"webkit-html-entity-value");}
var hasText=(forceValue||value.length>0);var attrSpanElement=parentElement.createChild("span","webkit-html-attribute");var attrNameElement=attrSpanElement.createChild("span","webkit-html-attribute-name");attrNameElement.textContent=name;if(hasText)
attrSpanElement.createTextChild("=\u200B\"");var attrValueElement=attrSpanElement.createChild("span","webkit-html-attribute-value");var updates=this._updateInfo;if(updates&&updates.isAttributeModified(name))
WebInspector.runCSSAnimationOnce(hasText?attrValueElement:attrNameElement,"dom-update-highlight");function linkifyValue(value)
{var rewrittenHref=node.resolveURL(value);if(rewrittenHref===null){var span=createElement("span");setValueWithEntities.call(this,span,value);return span;}
value=value.replace(closingPunctuationRegex,"$&\u200B");if(value.startsWith("data:"))
value=value.trimMiddle(60);var anchor=linkify(rewrittenHref,value,"",node.nodeName().toLowerCase()==="a");anchor.preventFollow=true;return anchor;}
if(linkify&&(name==="src"||name==="href")){attrValueElement.appendChild(linkifyValue.call(this,value));}else if(linkify&&node.nodeName().toLowerCase()==="img"&&name==="srcset"){var sources=value.split(",");for(var i=0;i<sources.length;++i){if(i>0)
attrValueElement.createTextChild(", ");var source=sources[i].trim();var indexOfSpace=source.indexOf(" ");var url=source.substring(0,indexOfSpace);var tail=source.substring(indexOfSpace);attrValueElement.appendChild(linkifyValue.call(this,url));attrValueElement.createTextChild(tail);}}else{setValueWithEntities.call(this,attrValueElement,value);}
if(hasText)
attrSpanElement.createTextChild("\"");},_buildPseudoElementDOM:function(parentElement,pseudoElementName)
{var pseudoElement=parentElement.createChild("span","webkit-html-pseudo-element");pseudoElement.textContent="::"+pseudoElementName;parentElement.createTextChild("\u200B");},_buildTagDOM:function(parentElement,tagName,isClosingTag,isDistinctTreeElement,linkify)
{var node=this._node;var classes=["webkit-html-tag"];if(isClosingTag&&isDistinctTreeElement)
classes.push("close");var tagElement=parentElement.createChild("span",classes.join(" "));tagElement.createTextChild("<");var tagNameElement=tagElement.createChild("span",isClosingTag?"":"webkit-html-tag-name");tagNameElement.textContent=(isClosingTag?"/":"")+tagName;if(!isClosingTag){if(node.hasAttributes()){var attributes=node.attributes();for(var i=0;i<attributes.length;++i){var attr=attributes[i];tagElement.createTextChild(" ");this._buildAttributeDOM(tagElement,attr.name,attr.value,false,node,linkify);}}
var hasUpdates;var updates=this._updateInfo;if(updates){hasUpdates|=updates.hasRemovedAttributes();var hasInlineText=this._childrenDisplayMode===WebInspector.ElementsTreeElement.ChildrenDisplayMode.InlineText;hasUpdates|=(!hasInlineText||this.expanded)&&updates.hasChangedChildren();hasUpdates|=!this.expanded&&updates.hasInsertedNodes()&&(!hasInlineText||this._node.firstChild.nodeValue().length===0);hasUpdates|=hasInlineText&&(updates.isCharDataModified()||updates.hasChangedChildren())&&this._node.firstChild.nodeValue().length===0;}
if(hasUpdates)
WebInspector.runCSSAnimationOnce(tagNameElement,"dom-update-highlight");}
tagElement.createTextChild(">");parentElement.createTextChild("\u200B");},_convertWhitespaceToEntities:function(text)
{var result="";var lastIndexAfterEntity=0;var entityRanges=[];var charToEntity=WebInspector.ElementsTreeOutline.MappedCharToEntity;for(var i=0,size=text.length;i<size;++i){var char=text.charAt(i);if(charToEntity[char]){result+=text.substring(lastIndexAfterEntity,i);var entityValue="&"+charToEntity[char]+";";entityRanges.push({offset:result.length,length:entityValue.length});result+=entityValue;lastIndexAfterEntity=i+1;}}
if(result)
result+=text.substring(lastIndexAfterEntity);return{text:result||text,entityRanges:entityRanges};},_nodeTitleInfo:function(linkify)
{var node=this._node;var info={titleDOM:createDocumentFragment(),hasChildren:this.hasChildren};switch(node.nodeType()){case Node.ATTRIBUTE_NODE:this._buildAttributeDOM(info.titleDOM,(node.name),(node.value),true);break;case Node.ELEMENT_NODE:var pseudoType=node.pseudoType();if(pseudoType){this._buildPseudoElementDOM(info.titleDOM,pseudoType);info.hasChildren=false;break;}
var tagName=node.nodeNameInCorrectCase();if(this._elementCloseTag){this._buildTagDOM(info.titleDOM,tagName,true,true);info.hasChildren=false;break;}
this._buildTagDOM(info.titleDOM,tagName,false,false,linkify);switch(this._childrenDisplayMode){case WebInspector.ElementsTreeElement.ChildrenDisplayMode.HasChildren:if(!this.expanded){var textNodeElement=info.titleDOM.createChild("span","webkit-html-text-node bogus");textNodeElement.textContent="\u2026";info.titleDOM.createTextChild("\u200B");this._buildTagDOM(info.titleDOM,tagName,true,false);}
break;case WebInspector.ElementsTreeElement.ChildrenDisplayMode.InlineText:var textNodeElement=info.titleDOM.createChild("span","webkit-html-text-node");var result=this._convertWhitespaceToEntities(node.firstChild.nodeValue());textNodeElement.textContent=result.text;WebInspector.highlightRangesWithStyleClass(textNodeElement,result.entityRanges,"webkit-html-entity-value");info.titleDOM.createTextChild("\u200B");info.hasChildren=false;this._buildTagDOM(info.titleDOM,tagName,true,false);var updates=this._updateInfo;if(updates&&(updates.hasInsertedNodes()||updates.hasChangedChildren()))
WebInspector.runCSSAnimationOnce(textNodeElement,"dom-update-highlight");updates=this._updateInfo;if(updates&&updates.isCharDataModified())
WebInspector.runCSSAnimationOnce(textNodeElement,"dom-update-highlight");break;case WebInspector.ElementsTreeElement.ChildrenDisplayMode.NoChildren:if(this.treeOutline.isXMLMimeType||!WebInspector.ElementsTreeElement.ForbiddenClosingTagElements[tagName])
this._buildTagDOM(info.titleDOM,tagName,true,false);break;}
break;case Node.TEXT_NODE:if(node.parentNode&&node.parentNode.nodeName().toLowerCase()==="script"){var newNode=info.titleDOM.createChild("span","webkit-html-text-node webkit-html-js-node");newNode.textContent=node.nodeValue();var javascriptSyntaxHighlighter=new WebInspector.DOMSyntaxHighlighter("text/javascript",true);javascriptSyntaxHighlighter.syntaxHighlightNode(newNode).then(updateSearchHighlight.bind(this));}else if(node.parentNode&&node.parentNode.nodeName().toLowerCase()==="style"){var newNode=info.titleDOM.createChild("span","webkit-html-text-node webkit-html-css-node");newNode.textContent=node.nodeValue();var cssSyntaxHighlighter=new WebInspector.DOMSyntaxHighlighter("text/css",true);cssSyntaxHighlighter.syntaxHighlightNode(newNode).then(updateSearchHighlight.bind(this));}else{info.titleDOM.createTextChild("\"");var textNodeElement=info.titleDOM.createChild("span","webkit-html-text-node");var result=this._convertWhitespaceToEntities(node.nodeValue());textNodeElement.textContent=result.text;WebInspector.highlightRangesWithStyleClass(textNodeElement,result.entityRanges,"webkit-html-entity-value");info.titleDOM.createTextChild("\"");var updates=this._updateInfo;if(updates&&updates.isCharDataModified())
WebInspector.runCSSAnimationOnce(textNodeElement,"dom-update-highlight");}
break;case Node.COMMENT_NODE:var commentElement=info.titleDOM.createChild("span","webkit-html-comment");commentElement.createTextChild("<!--"+node.nodeValue()+"-->");break;case Node.DOCUMENT_TYPE_NODE:var docTypeElement=info.titleDOM.createChild("span","webkit-html-doctype");docTypeElement.createTextChild("<!DOCTYPE "+node.nodeName());if(node.publicId){docTypeElement.createTextChild(" PUBLIC \""+node.publicId+"\"");if(node.systemId)
docTypeElement.createTextChild(" \""+node.systemId+"\"");}else if(node.systemId)
docTypeElement.createTextChild(" SYSTEM \""+node.systemId+"\"");if(node.internalSubset)
docTypeElement.createTextChild(" ["+node.internalSubset+"]");docTypeElement.createTextChild(">");break;case Node.CDATA_SECTION_NODE:var cdataElement=info.titleDOM.createChild("span","webkit-html-text-node");cdataElement.createTextChild("<![CDATA["+node.nodeValue()+"]]>");break;case Node.DOCUMENT_FRAGMENT_NODE:var fragmentElement=info.titleDOM.createChild("span","webkit-html-fragment");if(node.isInShadowTree()){var shadowRootType=node.shadowRootType();if(shadowRootType){info.shadowRoot=true;fragmentElement.classList.add("shadow-root");}}
fragmentElement.textContent=node.nodeNameInCorrectCase().collapseWhitespace();break;default:info.titleDOM.createTextChild(node.nodeNameInCorrectCase().collapseWhitespace());}
function updateSearchHighlight()
{delete this._highlightResult;this._highlightSearchResults();}
return info;},remove:function()
{if(this._node.pseudoType())
return;var parentElement=this.parent;if(!parentElement)
return;if(!this._node.parentNode||this._node.parentNode.nodeType()===Node.DOCUMENT_NODE)
return;this._node.removeNode();},toggleEditAsHTML:function(callback)
{if(this._editing&&this._htmlEditElement&&WebInspector.isBeingEdited(this._htmlEditElement)){this._editing.commit();return;}
function selectNode(error)
{callback(!error);}
function commitChange(initialValue,value)
{if(initialValue!==value)
node.setOuterHTML(value,selectNode);}
var node=this._node;node.getOuterHTML(this._startEditingAsHTML.bind(this,commitChange));},_copyCSSPath:function()
{InspectorFrontendHost.copyText(WebInspector.DOMPresentationUtils.cssPath(this._node,true));},_copyXPath:function()
{InspectorFrontendHost.copyText(WebInspector.DOMPresentationUtils.xPath(this._node,true));},_highlightSearchResults:function()
{if(!this._searchQuery||!this._searchHighlightsVisible)
return;this._hideSearchHighlight();var text=this.listItemElement.textContent;var regexObject=createPlainTextSearchRegex(this._searchQuery,"gi");var match=regexObject.exec(text);var matchRanges=[];while(match){matchRanges.push(new WebInspector.SourceRange(match.index,match[0].length));match=regexObject.exec(text);}
if(!matchRanges.length)
matchRanges.push(new WebInspector.SourceRange(0,text.length));this._highlightResult=[];WebInspector.highlightSearchResults(this.listItemElement,matchRanges,this._highlightResult);},_scrollIntoView:function()
{function scrollIntoViewCallback(object)
{function scrollIntoView()
{this.scrollIntoViewIfNeeded(true);}
if(object)
object.callFunction(scrollIntoView);}
this._node.resolveToObject("",scrollIntoViewCallback);},__proto__:TreeElement.prototype};WebInspector.ElementsTreeOutline=function(target,omitRootDOMNode,selectEnabled,setPseudoClassCallback)
{this._target=target;this._domModel=target.domModel;var element=createElement("div");this._shadowRoot=element.createShadowRoot();this._shadowRoot.appendChild(WebInspector.View.createStyleElement("elements/elementsTreeOutline.css"));this._elementsTreeUpdater=new WebInspector.ElementsTreeUpdater(this._domModel,this);var outlineDisclosureElement=this._shadowRoot.createChild("div","outline-disclosure");WebInspector.installComponentRootStyles(outlineDisclosureElement);this._element=outlineDisclosureElement.createChild("ol","elements-tree-outline source-code");this._element.addEventListener("mousedown",this._onmousedown.bind(this),false);this._element.addEventListener("mousemove",this._onmousemove.bind(this),false);this._element.addEventListener("mouseleave",this._onmouseleave.bind(this),false);this._element.addEventListener("dragstart",this._ondragstart.bind(this),false);this._element.addEventListener("dragover",this._ondragover.bind(this),false);this._element.addEventListener("dragleave",this._ondragleave.bind(this),false);this._element.addEventListener("drop",this._ondrop.bind(this),false);this._element.addEventListener("dragend",this._ondragend.bind(this),false);this._element.addEventListener("keydown",this._onkeydown.bind(this),false);this._element.addEventListener("webkitAnimationEnd",this._onAnimationEnd.bind(this),false);this._element.addEventListener("contextmenu",this._contextMenuEventFired.bind(this),false);TreeOutline.call(this,this._element);this.element=element;this._includeRootDOMNode=!omitRootDOMNode;this._selectEnabled=selectEnabled;this._rootDOMNode=null;this._selectedDOMNode=null;this._eventSupport=new WebInspector.Object();this._visible=false;this._pickNodeMode=false;this.setPseudoClassCallback=setPseudoClassCallback;this._createNodeDecorators();this._popoverHelper=new WebInspector.PopoverHelper(this._element,this._getPopoverAnchor.bind(this),this._showPopover.bind(this));this._popoverHelper.setTimeout(0);this._shadowHostDisplayModes=new WeakMap();}
WebInspector.ElementsTreeOutline.ClipboardData;WebInspector.ElementsTreeOutline.Events={NodePicked:"NodePicked",SelectedNodeChanged:"SelectedNodeChanged",ElementsTreeUpdated:"ElementsTreeUpdated"}
WebInspector.ElementsTreeOutline.MappedCharToEntity={"\u00a0":"nbsp","\u2002":"ensp","\u2003":"emsp","\u2009":"thinsp","\u200a":"#8202","\u200b":"#8203","\u200c":"zwnj","\u200d":"zwj","\u200e":"lrm","\u200f":"rlm","\u202a":"#8234","\u202b":"#8235","\u202c":"#8236","\u202d":"#8237","\u202e":"#8238"}
WebInspector.ElementsTreeOutline.ShadowHostDisplayMode={Composed:"Composed",Flattened:"Flattened",}
WebInspector.ElementsTreeOutline.prototype={focus:function()
{this._element.focus();},setWordWrap:function(wrap)
{this._element.classList.toggle("elements-tree-nowrap",!wrap);},_onAnimationEnd:function(event)
{event.target.classList.remove("elements-tree-element-pick-node-1");event.target.classList.remove("elements-tree-element-pick-node-2");},pickNodeMode:function()
{return this._pickNodeMode;},setPickNodeMode:function(value)
{this._pickNodeMode=value;this._element.classList.toggle("pick-node-mode",value);},handlePickNode:function(element,node)
{if(!this._pickNodeMode)
return false;this._eventSupport.dispatchEventToListeners(WebInspector.ElementsTreeOutline.Events.NodePicked,node);var hasRunningAnimation=element.classList.contains("elements-tree-element-pick-node-1")||element.classList.contains("elements-tree-element-pick-node-2");element.classList.toggle("elements-tree-element-pick-node-1");if(hasRunningAnimation)
element.classList.toggle("elements-tree-element-pick-node-2");return true;},target:function()
{return this._target;},domModel:function()
{return this._domModel;},setMultilineEditing:function(multilineEditing)
{this._multilineEditing=multilineEditing;},visibleWidth:function()
{return this._visibleWidth;},setVisibleWidth:function(width)
{this._visibleWidth=width;if(this._multilineEditing)
this._multilineEditing.setWidth(this._visibleWidth);},nodeDecorators:function()
{return this._nodeDecorators;},_createNodeDecorators:function()
{this._nodeDecorators=[];this._nodeDecorators.push(new WebInspector.ElementsTreeOutline.PseudoStateDecorator());},wireToDOMModel:function()
{this._elementsTreeUpdater._addDOMModelListeners();},unwireFromDOMModel:function()
{this._elementsTreeUpdater._removeDOMModelListeners();},_setClipboardData:function(data)
{if(this._clipboardNodeData){var treeElement=this.findTreeElement(this._clipboardNodeData.node);if(treeElement)
treeElement.setInClipboard(false);delete this._clipboardNodeData;}
if(data){var treeElement=this.findTreeElement(data.node);if(treeElement)
treeElement.setInClipboard(true);this._clipboardNodeData=data;}},resetClipboardIfNeeded:function(removedNode)
{if(this._clipboardNodeData&&this._clipboardNodeData.node===removedNode)
this._setClipboardData(null);},handleCopyOrCutKeyboardEvent:function(isCut,event)
{this._setClipboardData(null);if(!event.target.window().getSelection().isCollapsed)
return;if(WebInspector.isEditing())
return;var targetNode=this.selectedDOMNode();if(!targetNode)
return;event.clipboardData.clearData();event.preventDefault();this.performCopyOrCut(isCut,targetNode);},performCopyOrCut:function(isCut,node)
{if(isCut&&(node.isShadowRoot()||node.ancestorUserAgentShadowRoot()))
return;node.copyNode();this._setClipboardData({node:node,isCut:isCut});},canPaste:function(targetNode)
{if(targetNode.isShadowRoot()||targetNode.ancestorUserAgentShadowRoot())
return false;if(!this._clipboardNodeData)
return false;var node=this._clipboardNodeData.node;if(this._clipboardNodeData.isCut&&(node===targetNode||node.isAncestor(targetNode)))
return false;if(targetNode.target()!==node.target())
return false;return true;},pasteNode:function(targetNode)
{if(this.canPaste(targetNode))
this._performPaste(targetNode);},handlePasteKeyboardEvent:function(event)
{if(WebInspector.isEditing())
return;var targetNode=this.selectedDOMNode();if(!targetNode||!this.canPaste(targetNode))
return;event.preventDefault();this._performPaste(targetNode);},_performPaste:function(targetNode)
{if(this._clipboardNodeData.isCut){this._clipboardNodeData.node.moveTo(targetNode,null,expandCallback.bind(this));this._setClipboardData(null);}else{this._clipboardNodeData.node.copyTo(targetNode,null,expandCallback.bind(this));}
function expandCallback(error,nodeId)
{if(error)
return;var pastedNode=this._domModel.nodeForId(nodeId);if(!pastedNode)
return;this.selectDOMNode(pastedNode);}},setVisible:function(visible)
{this._visible=visible;if(!this._visible){this._popoverHelper.hidePopover();return;}
this.runPendingUpdates();if(this._selectedDOMNode)
this._revealAndSelectNode(this._selectedDOMNode,false);},addEventListener:function(eventType,listener,thisObject)
{this._eventSupport.addEventListener(eventType,listener,thisObject);},removeEventListener:function(eventType,listener,thisObject)
{this._eventSupport.removeEventListener(eventType,listener,thisObject);},get rootDOMNode()
{return this._rootDOMNode;},set rootDOMNode(x)
{if(this._rootDOMNode===x)
return;this._rootDOMNode=x;this._isXMLMimeType=x&&x.isXMLNode();this.update();},get isXMLMimeType()
{return this._isXMLMimeType;},selectedDOMNode:function()
{return this._selectedDOMNode;},selectDOMNode:function(node,focus)
{if(this._selectedDOMNode===node){this._revealAndSelectNode(node,!focus);return;}
this._selectedDOMNode=node;this._revealAndSelectNode(node,!focus);if(this._selectedDOMNode===node)
this._selectedNodeChanged();},editing:function()
{var node=this.selectedDOMNode();if(!node)
return false;var treeElement=this.findTreeElement(node);if(!treeElement)
return false;return treeElement.isEditing()||false;},update:function()
{var selectedNode=this.selectedTreeElement?this.selectedTreeElement.node():null;this.removeChildren();if(!this.rootDOMNode)
return;var treeElement;if(this._includeRootDOMNode){treeElement=this._elementsTreeUpdater._createElementTreeElement(this.rootDOMNode);this.appendChild(treeElement);}else{var node=this.rootDOMNode.firstChild;while(node){treeElement=this._elementsTreeUpdater._createElementTreeElement(node);this.appendChild(treeElement);node=node.nextSibling;}}
if(selectedNode)
this._revealAndSelectNode(selectedNode,true);},updateSelection:function()
{if(!this.selectedTreeElement)
return;var element=this.treeOutline.selectedTreeElement;element.updateSelection();},updateOpenCloseTags:function(node)
{var treeElement=this.findTreeElement(node);if(treeElement)
treeElement.updateTitle();var children=treeElement.children;var closingTagElement=children[children.length-1];if(closingTagElement&&closingTagElement.isClosingTag())
closingTagElement.updateTitle();},_selectedNodeChanged:function()
{this._eventSupport.dispatchEventToListeners(WebInspector.ElementsTreeOutline.Events.SelectedNodeChanged,this._selectedDOMNode);},_fireElementsTreeUpdated:function(nodes)
{this._eventSupport.dispatchEventToListeners(WebInspector.ElementsTreeOutline.Events.ElementsTreeUpdated,nodes);},findTreeElement:function(node)
{var treeElement=this._lookUpTreeElement(node);if(!treeElement&&node.nodeType()===Node.TEXT_NODE){treeElement=this._lookUpTreeElement(node.parentNode);}
return(treeElement);},_lookUpTreeElement:function(node)
{if(!node)
return null;var cachedElement=this.getCachedTreeElement(node);if(cachedElement)
return cachedElement;var ancestors=[];for(var currentNode=node.parentNode;currentNode;currentNode=currentNode.parentNode){ancestors.push(currentNode);if(this.getCachedTreeElement(currentNode))
break;}
if(!currentNode)
return null;for(var i=ancestors.length-1;i>=0;--i){var treeElement=this.getCachedTreeElement(ancestors[i]);if(treeElement)
treeElement.onpopulate();}
return this.getCachedTreeElement(node);},createTreeElementFor:function(node)
{var treeElement=this.findTreeElement(node);if(treeElement)
return treeElement;if(!node.parentNode)
return null;treeElement=this.createTreeElementFor(node.parentNode);return treeElement?this._elementsTreeUpdater._showChild(treeElement,node):null;},set suppressRevealAndSelect(x)
{if(this._suppressRevealAndSelect===x)
return;this._suppressRevealAndSelect=x;},_revealAndSelectNode:function(node,omitFocus)
{if(this._suppressRevealAndSelect)
return;if(!this._includeRootDOMNode&&node===this.rootDOMNode&&this.rootDOMNode)
node=this.rootDOMNode.firstChild;if(!node)
return;var treeElement=this.createTreeElementFor(node);if(!treeElement)
return;treeElement.revealAndSelect(omitFocus);},_treeElementFromEvent:function(event)
{var scrollContainer=this.element.parentElement;var x=scrollContainer.totalOffsetLeft()+scrollContainer.offsetWidth-36;var y=event.pageY;var elementUnderMouse=this.treeElementFromPoint(x,y);var elementAboveMouse=this.treeElementFromPoint(x,y-2);var element;if(elementUnderMouse===elementAboveMouse)
element=elementUnderMouse;else
element=this.treeElementFromPoint(x,y+2);return element;},_getPopoverAnchor:function(element,event)
{var anchor=element.enclosingNodeOrSelfWithClass("webkit-html-resource-link");if(!anchor||!anchor.href)
return;return anchor;},_loadDimensionsForNode:function(node,callback)
{if(!node.nodeName()||node.nodeName().toLowerCase()!=="img"){callback();return;}
node.resolveToObject("",resolvedNode);function resolvedNode(object)
{if(!object){callback();return;}
object.callFunctionJSON(features,undefined,callback);object.release();function features()
{return{offsetWidth:this.offsetWidth,offsetHeight:this.offsetHeight,naturalWidth:this.naturalWidth,naturalHeight:this.naturalHeight,currentSrc:this.currentSrc};}}},_showPopover:function(anchor,popover)
{var listItem=anchor.enclosingNodeOrSelfWithNodeName("li");var node=(listItem.treeElement).node();this._loadDimensionsForNode(node,WebInspector.DOMPresentationUtils.buildImagePreviewContents.bind(WebInspector.DOMPresentationUtils,node.target(),anchor.href,true,showPopover));function showPopover(contents)
{if(!contents)
return;popover.setCanShrink(false);popover.showForAnchor(contents,anchor);}},_onmousedown:function(event)
{var element=this._treeElementFromEvent(event);if(!element||element.isEventWithinDisclosureTriangle(event))
return;element.select();},_onmousemove:function(event)
{var element=this._treeElementFromEvent(event);if(element&&this._previousHoveredElement===element)
return;if(this._previousHoveredElement){this._previousHoveredElement.hovered=false;delete this._previousHoveredElement;}
if(element){element.hovered=true;this._previousHoveredElement=element;}
if(!(element instanceof WebInspector.ElementsTreeElement))
return;if(element&&element.node())
this._domModel.highlightDOMNodeWithConfig(element.node().id,{mode:"all",showInfo:!WebInspector.KeyboardShortcut.eventHasCtrlOrMeta(event)});else
this._domModel.hideDOMNodeHighlight();},_onmouseleave:function(event)
{if(this._previousHoveredElement){this._previousHoveredElement.hovered=false;delete this._previousHoveredElement;}
this._domModel.hideDOMNodeHighlight();},_ondragstart:function(event)
{if(!event.target.window().getSelection().isCollapsed)
return false;if(event.target.nodeName==="A")
return false;var treeElement=this._treeElementFromEvent(event);if(!this._isValidDragSourceOrTarget(treeElement))
return false;if(treeElement.node().nodeName()==="BODY"||treeElement.node().nodeName()==="HEAD")
return false;event.dataTransfer.setData("text/plain",treeElement.listItemElement.textContent.replace(/\u200b/g,""));event.dataTransfer.effectAllowed="copyMove";this._treeElementBeingDragged=treeElement;this._domModel.hideDOMNodeHighlight();return true;},_ondragover:function(event)
{if(!this._treeElementBeingDragged)
return false;var treeElement=this._treeElementFromEvent(event);if(!this._isValidDragSourceOrTarget(treeElement))
return false;var node=treeElement.node();while(node){if(node===this._treeElementBeingDragged._node)
return false;node=node.parentNode;}
treeElement.updateSelection();treeElement.listItemElement.classList.add("elements-drag-over");this._dragOverTreeElement=treeElement;event.preventDefault();event.dataTransfer.dropEffect='move';return false;},_ondragleave:function(event)
{this._clearDragOverTreeElementMarker();event.preventDefault();return false;},_isValidDragSourceOrTarget:function(treeElement)
{if(!treeElement)
return false;if(!(treeElement instanceof WebInspector.ElementsTreeElement))
return false;var elementsTreeElement=(treeElement);var node=elementsTreeElement.node();if(!node.parentNode||node.parentNode.nodeType()!==Node.ELEMENT_NODE)
return false;return true;},_ondrop:function(event)
{event.preventDefault();var treeElement=this._treeElementFromEvent(event);if(treeElement)
this._doMove(treeElement);},_doMove:function(treeElement)
{if(!this._treeElementBeingDragged)
return;var parentNode;var anchorNode;if(treeElement.isClosingTag()){parentNode=treeElement.node();}else{var dragTargetNode=treeElement.node();parentNode=dragTargetNode.parentNode;anchorNode=dragTargetNode;}
var wasExpanded=this._treeElementBeingDragged.expanded;this._treeElementBeingDragged._node.moveTo(parentNode,anchorNode,this.selectNodeAfterEdit.bind(this,wasExpanded));delete this._treeElementBeingDragged;},_ondragend:function(event)
{event.preventDefault();this._clearDragOverTreeElementMarker();delete this._treeElementBeingDragged;},_clearDragOverTreeElementMarker:function()
{if(this._dragOverTreeElement){this._dragOverTreeElement.updateSelection();this._dragOverTreeElement.listItemElement.classList.remove("elements-drag-over");delete this._dragOverTreeElement;}},_onkeydown:function(event)
{var keyboardEvent=(event);var node=(this.selectedDOMNode());console.assert(node);var treeElement=this.getCachedTreeElement(node);if(!treeElement)
return;if(!treeElement.isEditing()&&WebInspector.KeyboardShortcut.hasNoModifiers(keyboardEvent)&&keyboardEvent.keyCode===WebInspector.KeyboardShortcut.Keys.H.code){this._toggleHideShortcut(node);event.consume(true);return;}},_contextMenuEventFired:function(event)
{var treeElement=this._treeElementFromEvent(event);if(!treeElement||WebInspector.isEditing())
return;var contextMenu=new WebInspector.ContextMenu(event);var isPseudoElement=!!treeElement.node().pseudoType();var isTag=treeElement.node().nodeType()===Node.ELEMENT_NODE&&!isPseudoElement;var textNode=event.target.enclosingNodeOrSelfWithClass("webkit-html-text-node");if(textNode&&textNode.classList.contains("bogus"))
textNode=null;var commentNode=event.target.enclosingNodeOrSelfWithClass("webkit-html-comment");contextMenu.appendApplicableItems(event.target);if(textNode){contextMenu.appendSeparator();treeElement.populateTextContextMenu(contextMenu,textNode);}else if(isTag){contextMenu.appendSeparator();treeElement.populateTagContextMenu(contextMenu,event);}else if(commentNode){contextMenu.appendSeparator();treeElement.populateNodeContextMenu(contextMenu);}else if(isPseudoElement){treeElement.populateScrollIntoView(contextMenu);}
contextMenu.appendApplicableItems(treeElement.node());contextMenu.show();},populateTreeElement:function(treeElement)
{if(this._elementsTreeUpdater)
this._elementsTreeUpdater._populateTreeElement(treeElement);},runPendingUpdates:function()
{if(this._elementsTreeUpdater)
this._elementsTreeUpdater._updateModifiedNodes();},handleShortcut:function(event)
{var node=this.selectedDOMNode();var treeElement=this.getCachedTreeElement(node);if(!node||!treeElement)
return;if(event.keyIdentifier==="F2"&&treeElement.hasEditableNode()){this._toggleEditAsHTML(node);event.handled=true;return;}
if(WebInspector.KeyboardShortcut.eventHasCtrlOrMeta(event)&&node.parentNode){if(event.keyIdentifier==="Up"&&node.previousSibling){node.moveTo(node.parentNode,node.previousSibling,this.selectNodeAfterEdit.bind(this,treeElement.expanded));event.handled=true;return;}
if(event.keyIdentifier==="Down"&&node.nextSibling){node.moveTo(node.parentNode,node.nextSibling.nextSibling,this.selectNodeAfterEdit.bind(this,treeElement.expanded));event.handled=true;return;}}},_toggleEditAsHTML:function(node)
{var treeElement=this.getCachedTreeElement(node);if(!treeElement)
return;if(node.pseudoType())
return;var parentNode=node.parentNode;var index=node.index;var wasExpanded=treeElement.expanded;treeElement.toggleEditAsHTML(editingFinished.bind(this));function editingFinished(success)
{if(!success)
return;this.runPendingUpdates();var newNode=parentNode?parentNode.children()[index]||parentNode:null;if(!newNode)
return;this.selectDOMNode(newNode,true);if(wasExpanded){var newTreeItem=this.findTreeElement(newNode);if(newTreeItem)
newTreeItem.expand();}}},selectNodeAfterEdit:function(wasExpanded,error,nodeId)
{if(error)
return null;this.runPendingUpdates();var newNode=nodeId?this._domModel.nodeForId(nodeId):null;if(!newNode)
return null;this.selectDOMNode(newNode,true);var newTreeItem=this.findTreeElement(newNode);if(wasExpanded){if(newTreeItem)
newTreeItem.expand();}
return newTreeItem;},_toggleHideShortcut:function(node,userCallback)
{var pseudoType=node.pseudoType();var effectiveNode=pseudoType?node.parentNode:node;if(!effectiveNode)
return;function resolvedNode(object)
{if(!object)
return;function toggleClassAndInjectStyleRule(pseudoType)
{const classNamePrefix="__web-inspector-hide";const classNameSuffix="-shortcut__";const styleTagId="__web-inspector-hide-shortcut-style__";var selectors=[];selectors.push("html /deep/ .__web-inspector-hide-shortcut__");selectors.push("html /deep/ .__web-inspector-hide-shortcut__ /deep/ *");selectors.push("html /deep/ .__web-inspector-hidebefore-shortcut__::before");selectors.push("html /deep/ .__web-inspector-hideafter-shortcut__::after");var selector=selectors.join(", ");var ruleBody="    visibility: hidden !important;";var rule="\n"+selector+"\n{\n"+ruleBody+"\n}\n";var className=classNamePrefix+(pseudoType||"")+classNameSuffix;this.classList.toggle(className);var style=document.head.querySelector("style#"+styleTagId);if(style)
return;style=document.createElement("style");style.id=styleTagId;style.type="text/css";style.textContent=rule;document.head.appendChild(style);}
object.callFunction(toggleClassAndInjectStyleRule,[{value:pseudoType}],userCallback);object.release();}
effectiveNode.resolveToObject("",resolvedNode);},__proto__:TreeOutline.prototype}
WebInspector.ElementsTreeOutline.ElementDecorator=function()
{}
WebInspector.ElementsTreeOutline.ElementDecorator.prototype={decorate:function(node)
{},decorateAncestor:function(node)
{}}
WebInspector.ElementsTreeOutline.PseudoStateDecorator=function()
{WebInspector.ElementsTreeOutline.ElementDecorator.call(this);}
WebInspector.ElementsTreeOutline.PseudoStateDecorator.prototype={decorate:function(node)
{if(node.nodeType()!==Node.ELEMENT_NODE)
return null;var propertyValue=node.getUserProperty(WebInspector.CSSStyleModel.PseudoStatePropertyName);if(!propertyValue)
return null;return WebInspector.UIString("Element state: %s",":"+propertyValue.join(", :"));},decorateAncestor:function(node)
{if(node.nodeType()!==Node.ELEMENT_NODE)
return null;var descendantCount=node.descendantUserPropertyCount(WebInspector.CSSStyleModel.PseudoStatePropertyName);if(!descendantCount)
return null;if(descendantCount===1)
return WebInspector.UIString("%d descendant with forced state",descendantCount);return WebInspector.UIString("%d descendants with forced state",descendantCount);}}
WebInspector.ElementsTreeUpdater=function(domModel,treeOutline)
{this._domModel=domModel;this._treeOutline=treeOutline;this._recentlyModifiedNodes=new Set();this._recentlyModifiedParentNodes=new Set();this._updateInfos=new Map();this._treeElementsBeingUpdated=new Set();}
WebInspector.ElementsTreeUpdater.prototype={_addDOMModelListeners:function()
{this._domModel.addEventListener(WebInspector.DOMModel.Events.NodeInserted,this._nodeInserted,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.NodeRemoved,this._nodeRemoved,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.AttrModified,this._attributeModified,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.AttrRemoved,this._attributeRemoved,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.CharacterDataModified,this._characterDataModified,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.DocumentUpdated,this._documentUpdated,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.ChildNodeCountUpdated,this._childNodeCountUpdated,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.DistributedNodesChanged,this._distributedNodesChanged,this);},_removeDOMModelListeners:function()
{this._domModel.removeEventListener(WebInspector.DOMModel.Events.NodeInserted,this._nodeInserted,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.NodeRemoved,this._nodeRemoved,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.AttrModified,this._attributeModified,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.AttrRemoved,this._attributeRemoved,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.CharacterDataModified,this._characterDataModified,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.DocumentUpdated,this._documentUpdated,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.ChildNodeCountUpdated,this._childNodeCountUpdated,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.DistributedNodesChanged,this._distributedNodesChanged,this);},_distributedNodesChanged:function(event)
{var shadowHost=(event.data);var shadowHostDisplayMode=this._treeOutline._shadowHostDisplayModes.get(shadowHost);if(!shadowHostDisplayMode)
return;var insertionPoints=shadowHost.insertionPoints();var shadowRoots=shadowHost.shadowRoots();this._parentNodeModified(shadowHost);for(var shadowRoot of shadowRoots){if(shadowHostDisplayMode===WebInspector.ElementsTreeOutline.ShadowHostDisplayMode.Composed)
this._parentNodeModified(shadowRoot);}
for(var insertionPoint of insertionPoints){if(shadowHostDisplayMode===WebInspector.ElementsTreeOutline.ShadowHostDisplayMode.Flattened)
this._parentNodeModified(insertionPoint.parentNode);else
this._parentNodeModified(insertionPoint);}
this._updateModifiedNodesSoon();},_updateRecord:function(node)
{if(!WebInspector.settings.highlightDOMUpdates.get()||this._domUpdateHighlightsMuted)
return new WebInspector.ElementsTreeUpdater.UpdateInfo();var record=this._updateInfos.get(node);if(!record){record=new WebInspector.ElementsTreeUpdater.UpdateInfo();this._updateInfos.set(node,record);}
return record;},_updateInfo:function(node)
{if(!WebInspector.settings.highlightDOMUpdates.get())
return null;return this._updateInfos.get(node)||null;},_parentNodeModified:function(parentNode)
{if(!parentNode)
return new WebInspector.ElementsTreeUpdater.UpdateInfo();this._recentlyModifiedParentNodes.add(parentNode);var treeElement=this._treeOutline.findTreeElement(parentNode);if(treeElement){var oldDisplayMode=treeElement.childrenDisplayMode();this._updateChildrenDisplayMode(treeElement);if(treeElement.childrenDisplayMode()!==oldDisplayMode){this._nodeModified(parentNode);this._updateRecord(parentNode).childrenModified();}}
return this._updateRecord(parentNode);},_nodeModified:function(node)
{this._recentlyModifiedNodes.add(node);return this._updateRecord(node);},_documentUpdated:function(event)
{var inspectedRootDocument=event.data;this._reset();if(!inspectedRootDocument)
return;this._treeOutline.rootDOMNode=inspectedRootDocument;},_attributeModified:function(event)
{var node=(event.data.node);this._nodeModified(node).attributeModified(event.data.name);this._updateModifiedNodesSoon();},_attributeRemoved:function(event)
{var node=(event.data.node);this._nodeModified(node).attributeRemoved(event.data.name);this._updateModifiedNodesSoon();},_characterDataModified:function(event)
{var node=(event.data);this._parentNodeModified(node.parentNode).charDataModified();this._nodeModified(node).charDataModified();this._updateModifiedNodesSoon();},_nodeInserted:function(event)
{var node=(event.data);this._parentNodeModified(node.parentNode).nodeInserted(node);this._updateModifiedNodesSoon();},_nodeRemoved:function(event)
{var node=(event.data.node);var parentNode=(event.data.parent);this._treeOutline.resetClipboardIfNeeded(node);this._parentNodeModified(parentNode).childrenModified();this._updateModifiedNodesSoon();},_childNodeCountUpdated:function(event)
{var node=(event.data);this._parentNodeModified(node);this._updateModifiedNodesSoon();},_setUpdateInfos:function()
{for(var node of this._updateInfos.keys()){var treeElement=this._treeOutline.getCachedTreeElement(node);if(treeElement)
treeElement.setUpdateInfo(this._updateInfo(node));}},_clearUpdateInfos:function()
{for(var node of this._updateInfos.keys()){var treeElement=this._treeOutline.getCachedTreeElement(node);if(treeElement)
treeElement.setUpdateInfo(null);}
this._updateInfos.clear();},_updateModifiedNodesSoon:function()
{if(!this._recentlyModifiedNodes.size&&!this._recentlyModifiedParentNodes.size)
return;if(!this._treeOutline._visible){this._updateInfos.clear();return;}
if(this._updateModifiedNodesTimeout)
return;this._updateModifiedNodesTimeout=setTimeout(this._updateModifiedNodes.bind(this),50);},_updateModifiedNodes:function()
{if(this._updateModifiedNodesTimeout){clearTimeout(this._updateModifiedNodesTimeout);delete this._updateModifiedNodesTimeout;}
this._setUpdateInfos();var updatedNodes=new Set();for(var node of this._recentlyModifiedNodes)
updatedNodes.add(node);for(var node of this._recentlyModifiedParentNodes)
updatedNodes.add(node);var hidePanelWhileUpdating=updatedNodes.length>10;if(hidePanelWhileUpdating){var treeOutlineContainerElement=this._treeOutline.element.parentNode;var originalScrollTop=treeOutlineContainerElement?treeOutlineContainerElement.scrollTop:0;this._treeOutline._element.classList.add("hidden");}
if(this._treeOutline._rootDOMNode&&this._recentlyModifiedParentNodes.has(this._treeOutline._rootDOMNode)){this._treeOutline.update();}else{for(var node of this._recentlyModifiedNodes){var nodeItem=this._treeOutline.findTreeElement(node);if(nodeItem)
nodeItem.updateTitle(false);}
for(var node of this._recentlyModifiedParentNodes){var parentNodeItem=this._treeOutline.findTreeElement(node);if(parentNodeItem&&parentNodeItem.populated)
this.updateChildren(parentNodeItem);}}
if(hidePanelWhileUpdating){this._treeOutline._element.classList.remove("hidden");if(originalScrollTop)
treeOutlineContainerElement.scrollTop=originalScrollTop;this._treeOutline.updateSelection();}
this._clearUpdateInfos();this._recentlyModifiedNodes.clear();this._recentlyModifiedParentNodes.clear();this._treeOutline._fireElementsTreeUpdated(updatedNodes.valuesArray());},_reset:function()
{this._treeOutline.rootDOMNode=null;this._treeOutline.selectDOMNode(null,false);this._domModel.hideDOMNodeHighlight();this._recentlyModifiedNodes.clear();this._recentlyModifiedParentNodes.clear();this._updateInfos.clear();delete this._treeOutline._clipboardNodeData;},_populateTreeElement:function(treeElement)
{if(treeElement.children.length||!treeElement.hasChildren)
return;this.updateChildren(treeElement);},_createElementTreeElement:function(node,closingTag)
{var treeElement=new WebInspector.ElementsTreeElement(node,closingTag);treeElement.selectable=this._treeOutline._selectEnabled;if(!closingTag)
treeElement.setUpdateInfo(this._updateInfo(node));this._updateChildrenDisplayMode(treeElement);return treeElement;},_showChild:function(treeElement,child)
{if(treeElement.isClosingTag())
return null;var index=this._visibleChildren(treeElement.node()).indexOf(child);if(index===-1)
return null;if(index>=treeElement.expandedChildrenLimit())
this.setExpandedChildrenLimit(treeElement,index+1);if(treeElement.shadowHostToolbarElement)
++index;return(treeElement.children[index]);},_visibleShadowRoots:function(node)
{var roots=node.shadowRoots();if(roots.length&&!WebInspector.settings.showUAShadowDOM.get()){roots=roots.filter(function(root){return root.shadowRootType()===WebInspector.DOMNode.ShadowRootTypes.Author;});}
return roots;},_isShadowHostInComposedMode:function(node)
{var shadowRoots=this._visibleShadowRoots(node);return this._treeOutline._shadowHostDisplayModes.has(node)&&!!shadowRoots.length;},_isInsertionPointInComposedMode:function(node)
{var ancestorShadowHost=node.ancestorShadowHost();var isInShadowTreeInComposedMode=!!ancestorShadowHost&&this._treeOutline._shadowHostDisplayModes.has(ancestorShadowHost);return isInShadowTreeInComposedMode&&node.isInsertionPoint();},_shouldFlatten:function(node)
{var ancestorShadowHost=node.ancestorShadowHost();if(!ancestorShadowHost)
return false;if(this._treeOutline._shadowHostDisplayModes.get(ancestorShadowHost)!==WebInspector.ElementsTreeOutline.ShadowHostDisplayMode.Flattened)
return false;return node.isShadowRoot()||node.isInsertionPoint();},_visibleChildren:function(node)
{var result=[];var unflattenedChildren=this._unflattenedChildren(node);for(var child of unflattenedChildren){if(this._shouldFlatten(child)){var flattenedChildren=this._visibleChildren(child);result=result.concat(flattenedChildren);}else{result.push(child);}}
return result;},_unflattenedChildren:function(node)
{if(this._isShadowHostInComposedMode(node))
return this._visibleChildrenForShadowHostInComposedMode(node);if(this._isInsertionPointInComposedMode(node))
return this._visibleChildrenForInsertionPointInComposedMode(node);var visibleChildren=this._visibleShadowRoots(node);if(node.importedDocument())
visibleChildren.push(node.importedDocument());if(node.templateContent())
visibleChildren.push(node.templateContent());var beforePseudoElement=node.beforePseudoElement();if(beforePseudoElement)
visibleChildren.push(beforePseudoElement);if(node.childNodeCount())
visibleChildren=visibleChildren.concat(node.children());var afterPseudoElement=node.afterPseudoElement();if(afterPseudoElement)
visibleChildren.push(afterPseudoElement);return visibleChildren;},_hasVisibleChildren:function(node)
{if(this._isShadowHostInComposedMode(node))
return this._hasVisibleChildrenAsShadowHostInComposedMode(node);if(this._isInsertionPointInComposedMode(node))
return this._hasVisibleChildrenAsInsertionPointInComposedMode(node);if(node.importedDocument())
return true;if(node.templateContent())
return true;if(node.childNodeCount())
return true;if(this._visibleShadowRoots(node).length)
return true;if(node.hasPseudoElements())
return true;return false;},_visibleChildrenForShadowHostInComposedMode:function(node)
{var visibleChildren=[];var pseudoElements=node.pseudoElements();if(pseudoElements[WebInspector.DOMNode.PseudoElementNames.Before])
visibleChildren.push(pseudoElements[WebInspector.DOMNode.PseudoElementNames.Before]);var shadowRoots=this._visibleShadowRoots(node);if(shadowRoots.length)
visibleChildren.push(shadowRoots[0]);if(pseudoElements[WebInspector.DOMNode.PseudoElementNames.After])
visibleChildren.push(pseudoElements[WebInspector.DOMNode.PseudoElementNames.After]);return visibleChildren;},_hasVisibleChildrenAsShadowHostInComposedMode:function(node)
{if(this._visibleShadowRoots(node).length)
return true;if(node.hasPseudoElements())
return true;return false;},_visibleChildrenForInsertionPointInComposedMode:function(node)
{var visibleChildren=[];var pseudoElements=node.pseudoElements();if(pseudoElements[WebInspector.DOMNode.PseudoElementNames.Before])
visibleChildren.push(pseudoElements[WebInspector.DOMNode.PseudoElementNames.Before]);var distributedShadowRoot=node.distributedShadowRoot();if(distributedShadowRoot){visibleChildren.push(distributedShadowRoot)}else{var distributedNodes=node.distributedNodes();if(distributedNodes)
visibleChildren=visibleChildren.concat(distributedNodes);}
if(pseudoElements[WebInspector.DOMNode.PseudoElementNames.After])
visibleChildren.push(pseudoElements[WebInspector.DOMNode.PseudoElementNames.After]);return visibleChildren;},_hasVisibleChildrenAsInsertionPointInComposedMode:function(node)
{if(node.hasPseudoElements())
return true;var distributedShadowRoot=node.distributedShadowRoot();if(distributedShadowRoot)
return true;var distributedNodes=node.distributedNodes();if(distributedNodes&&distributedNodes.length)
return true;return false;},_canShowInlineText:function(treeElement)
{var node=treeElement.node();if(node.importedDocument()||node.templateContent()||this._visibleShadowRoots(node).length>0||node.hasPseudoElements())
return false;if(node.nodeType()!==Node.ELEMENT_NODE)
return false;if(!node.firstChild||node.firstChild!==node.lastChild||node.firstChild.nodeType()!==Node.TEXT_NODE)
return false;var textChild=node.firstChild;var maxInlineTextChildLength=80;if(textChild.nodeValue().length<maxInlineTextChildLength)
return true;return false;},_updateChildrenDisplayMode:function(treeElement)
{var node=treeElement.node();var showInlineText=this._canShowInlineText(treeElement);var hasChildren=!treeElement.isClosingTag()&&this._hasVisibleChildren(node);var childrenDisplayMode;if(showInlineText)
childrenDisplayMode=WebInspector.ElementsTreeElement.ChildrenDisplayMode.InlineText;else if(hasChildren)
childrenDisplayMode=WebInspector.ElementsTreeElement.ChildrenDisplayMode.HasChildren;else
childrenDisplayMode=WebInspector.ElementsTreeElement.ChildrenDisplayMode.NoChildren;treeElement.setChildrenDisplayMode(childrenDisplayMode);treeElement.setHasChildren(childrenDisplayMode===WebInspector.ElementsTreeElement.ChildrenDisplayMode.HasChildren);},_createExpandAllButtonTreeElement:function(treeElement)
{var button=createTextButton("",handleLoadAllChildren.bind(this));button.value="";var expandAllButtonElement=new TreeElement(button,null,false);expandAllButtonElement.selectable=false;expandAllButtonElement.expandAllButton=true;expandAllButtonElement.button=button;return expandAllButtonElement;function handleLoadAllChildren(event)
{var visibleChildCount=this._visibleChildren(treeElement.node()).length;this.setExpandedChildrenLimit(treeElement,Math.max(visibleChildCount,treeElement.expandedChildrenLimit()+WebInspector.ElementsTreeElement.InitialChildrenLimit));event.consume();}},setExpandedChildrenLimit:function(treeElement,expandedChildrenLimit)
{if(treeElement.expandedChildrenLimit()===expandedChildrenLimit)
return;treeElement.setExpandedChildrenLimit(expandedChildrenLimit);if(treeElement.treeOutline&&!this._treeElementsBeingUpdated.has(treeElement))
this._updateChildren(treeElement);},updateChildren:function(treeElement)
{if(!treeElement.hasChildren)
return;console.assert(!treeElement.isClosingTag());var barrier=new CallbackBarrier();treeElement.node().getChildNodes(childNodesLoaded.bind(null,barrier.createCallback()));var ancestorShadowHost=treeElement.node().ancestorShadowHost();var shouldLoadDistributedNodes=ancestorShadowHost&&this._treeOutline._shadowHostDisplayModes.has(ancestorShadowHost);if(shouldLoadDistributedNodes)
treeElement.node().ensureShadowHostDistributedNodesLoaded(barrier.createCallback());barrier.callWhenDone(this._updateChildren.bind(this,treeElement));function childNodesLoaded(callback,children)
{if(!children)
return;callback();}},insertChildElement:function(treeElement,child,index,closingTag)
{var newElement=this._createElementTreeElement(child,closingTag);treeElement.insertChild(newElement,index);return newElement;},moveChild:function(treeElement,child,targetIndex)
{var wasSelected=child.selected;treeElement.removeChild(child);treeElement.insertChild(child,targetIndex);if(wasSelected)
child.select();},_updateChildren:function(treeElement)
{if(this._treeElementsBeingUpdated.has(treeElement)||!this._treeOutline._visible)
return;var node=treeElement.node();this._treeElementsBeingUpdated.add(treeElement);var selectedTreeElement=treeElement.treeOutline.selectedTreeElement;var visibleChildren=this._visibleChildren(treeElement.node());var visibleChildrenSet=new Set(visibleChildren);var existingTreeElements=new Map();for(var i=treeElement.children.length-1;i>=0;--i){var existingTreeElement=treeElement.children[i];if(!(existingTreeElement instanceof WebInspector.ElementsTreeElement)){treeElement.removeChildAtIndex(i);continue;}
var elementsTreeElement=(existingTreeElement);var existingNode=elementsTreeElement.node();if(visibleChildrenSet.has(existingNode)){existingTreeElements.set(existingNode,existingTreeElement);continue;}
if(selectedTreeElement&&(selectedTreeElement===existingTreeElement||selectedTreeElement.hasAncestor(existingTreeElement))){if(existingTreeElement.nextSibling)
selectedTreeElement=existingTreeElement.nextSibling;else if(existingTreeElement.previousSibling)
selectedTreeElement=existingTreeElement.previousSibling;else
selectedTreeElement=treeElement;}
treeElement.removeChildAtIndex(i);}
if(selectedTreeElement!==treeElement.treeOutline.selectedTreeElement)
selectedTreeElement.select();for(var i=0;i<visibleChildren.length&&i<treeElement.expandedChildrenLimit();++i){var child=visibleChildren[i];if(existingTreeElements.has(child)){this.moveChild(treeElement,existingTreeElements.get(child),i);}else{var newElement=this.insertChildElement(treeElement,child,i);if(this._updateInfo(treeElement.node()))
WebInspector.ElementsTreeElement.animateOnDOMUpdate(newElement);if(treeElement.children.length>treeElement.expandedChildrenLimit())
this.setExpandedChildrenLimit(treeElement,treeElement.expandedChildrenLimit()+1);}}
treeElement.updateTitle();var expandedChildCount=treeElement.children.length;if(visibleChildren.length>expandedChildCount){var targetButtonIndex=expandedChildCount;if(!treeElement.expandAllButtonElement)
treeElement.expandAllButtonElement=this._createExpandAllButtonTreeElement(treeElement);treeElement.insertChild(treeElement.expandAllButtonElement,targetButtonIndex);treeElement.expandAllButtonElement.button.textContent=WebInspector.UIString("Show All Nodes (%d More)",visibleChildren.length-expandedChildCount);}else if(treeElement.expandAllButtonElement){delete treeElement.expandAllButtonElement;}
if(node.nodeType()===Node.ELEMENT_NODE&&treeElement.hasChildren)
this.insertChildElement(treeElement,node,treeElement.children.length,true);if(Runtime.experiments.isEnabled("composedShadowDOM")&&node.shadowRoots().length){if(!treeElement.shadowHostToolbarElement)
treeElement.shadowHostToolbarElement=this._createShadowHostToolbar(treeElement);treeElement.insertChild(treeElement.shadowHostToolbarElement,0);}else if(treeElement.shadowHostToolbarElement){delete treeElement.shadowHostToolbarElement;}
this._treeElementsBeingUpdated.delete(treeElement)},_setShadowHostDisplayMode:function(elementsTreeElement,newMode)
{var node=elementsTreeElement.node();var oldMode=this._treeOutline._shadowHostDisplayModes.has(node);if(newMode)
this._treeOutline._shadowHostDisplayModes.set(node,newMode);else
this._treeOutline._shadowHostDisplayModes.delete(node);if(elementsTreeElement.populated)
node.ensureShadowHostDistributedNodesLoaded(invalidateChildren.bind(this));else
invalidateChildren.call(this);function invalidateChildren()
{this._domUpdateHighlightsMuted=true;this._parentNodeModified(node);for(var shadowRoot of node.shadowRoots())
this._parentNodeModified(shadowRoot);for(var insertionPoint of node.insertionPoints())
this._parentNodeModified(insertionPoint);delete this._domUpdateHighlightsMuted;this._updateModifiedNodes();if(newMode===WebInspector.ElementsTreeOutline.ShadowHostDisplayMode.Composed){for(var shadowRoot of node.shadowRoots()){var treeElement=this._treeOutline.findTreeElement(shadowRoot);if(treeElement)
treeElement.expand();}
for(var insertionPoint of node.insertionPoints()){var treeElement=this._treeOutline.findTreeElement(insertionPoint);if(treeElement)
treeElement.expand();}}}
elementsTreeElement.shadowHostToolbarElement.updateButtons(newMode);},_createShadowHostToolbar:function(elementsTreeElement)
{function createButton(label,tooltip,mode)
{var button=createElement("button");button.className="shadow-host-display-mode-toolbar-button";button.textContent=label;button.title=tooltip;button.mode=mode;if(mode)
button.classList.add("custom-mode")
button.addEventListener("click",buttonClicked.bind(this));toolbar.appendChild(button);return button;}
function updateButtons(mode)
{for(var i=0;i<buttons.length;++i){var currentModeButton=mode===buttons[i].mode;buttons[i].classList.toggle("toggled",currentModeButton);buttons[i].disabled=currentModeButton;}}
function buttonClicked(event)
{var button=event.target;if(button.disabled)
return;this._setShadowHostDisplayMode(elementsTreeElement,button.mode);event.consume();}
var node=elementsTreeElement.node();var toolbar=createElementWithClass("div","shadow-host-display-mode-toolbar");var toolbarTreeElement=new TreeElement(toolbar,null,false);var buttons=[];buttons.push(createButton.call(this,"Logical",WebInspector.UIString("Logical view \n(Light and Shadow DOM are shown separately)."),null));buttons.push(createButton.call(this,"Composed",WebInspector.UIString("Composed view\n(Light DOM is shown as distributed into Shadow DOM)."),WebInspector.ElementsTreeOutline.ShadowHostDisplayMode.Composed));buttons.push(createButton.call(this,"Flattened",WebInspector.UIString("Flattened view\n(Same as composed view, but shadow roots and insertion points are hidden)."),WebInspector.ElementsTreeOutline.ShadowHostDisplayMode.Flattened));updateButtons(this._treeOutline._shadowHostDisplayModes.get(node)||null);toolbarTreeElement.selectable=false;toolbarTreeElement.shadowHostToolbar=true;toolbarTreeElement.buttons=buttons;toolbarTreeElement.updateButtons=updateButtons;return toolbarTreeElement;},}
WebInspector.ElementsTreeUpdater.UpdateInfo=function()
{}
WebInspector.ElementsTreeUpdater.UpdateInfo.prototype={attributeModified:function(attrName)
{if(this._removedAttributes&&this._removedAttributes.has(attrName))
this._removedAttributes.delete(attrName);if(!this._modifiedAttributes)
this._modifiedAttributes=(new Set());this._modifiedAttributes.add(attrName);},attributeRemoved:function(attrName)
{if(this._modifiedAttributes&&this._modifiedAttributes.has(attrName))
this._modifiedAttributes.delete(attrName);if(!this._removedAttributes)
this._removedAttributes=(new Set());this._removedAttributes.add(attrName);},nodeInserted:function(node)
{if(!this._insertedNodes)
this._insertedNodes=(new Set());this._insertedNodes.add((node));},charDataModified:function()
{this._charDataModified=true;},childrenModified:function()
{this._hasChangedChildren=true;},isAttributeModified:function(attributeName)
{return this._modifiedAttributes&&this._modifiedAttributes.has(attributeName);},hasRemovedAttributes:function()
{return!!this._removedAttributes&&!!this._removedAttributes.size;},hasInsertedNodes:function()
{return!!this._insertedNodes&&!!this._insertedNodes.size;},isCharDataModified:function()
{return!!this._charDataModified;},isNodeInserted:function(node)
{return!!this._insertedNodes&&this._insertedNodes.has(node);},hasChangedChildren:function()
{return!!this._hasChangedChildren;}}
WebInspector.ElementsTreeOutline.Renderer=function()
{}
WebInspector.ElementsTreeOutline.Renderer.prototype={render:function(object)
{return new Promise(renderPromise);function renderPromise(resolve,reject)
{if(object instanceof WebInspector.DOMNode)
onNodeResolved((object));else if(object instanceof WebInspector.DeferredDOMNode)
((object)).resolve(onNodeResolved);else if(object instanceof WebInspector.RemoteObject)
((object)).pushNodeToFrontend(onNodeResolved);else
reject(new Error("Can't reveal not a node."));function onNodeResolved(node)
{if(!node){reject(new Error("Could not resolve node."));return;}
var treeOutline=new WebInspector.ElementsTreeOutline(node.target(),false,false);treeOutline.rootDOMNode=node;if(!treeOutline.children[0].hasChildren)
treeOutline._element.classList.add("single-node");treeOutline.setVisible(true);treeOutline.element.treeElementForTest=treeOutline.children[0];resolve(treeOutline.element);}}}};WebInspector.EventListenersSidebarPane=function()
{WebInspector.ElementsSidebarPane.call(this,WebInspector.UIString("Event Listeners"));this.bodyElement.classList.add("events-pane");this.sections=[];var refreshButton=this.titleElement.createChild("button","pane-title-button refresh");refreshButton.addEventListener("click",this.update.bind(this),false);refreshButton.title=WebInspector.UIString("Refresh");this.settingsSelectElement=this.titleElement.createChild("select","select-filter");var option=this.settingsSelectElement.createChild("option");option.value="all";option.label=WebInspector.UIString("All Nodes");option=this.settingsSelectElement.createChild("option");option.value="selected";option.label=WebInspector.UIString("Selected Node Only");var filter=WebInspector.settings.eventListenersFilter.get();if(filter==="all")
this.settingsSelectElement[0].selected=true;else if(filter==="selected")
this.settingsSelectElement[1].selected=true;this.settingsSelectElement.addEventListener("click",consumeEvent,false);this.settingsSelectElement.addEventListener("change",this._changeSetting.bind(this),false);this._linkifier=new WebInspector.Linkifier();}
WebInspector.EventListenersSidebarPane._objectGroupName="event-listeners-sidebar-pane";WebInspector.EventListenersSidebarPane.prototype={doUpdate:function(finishCallback)
{if(this._lastRequestedNode){this._lastRequestedNode.target().runtimeAgent().releaseObjectGroup(WebInspector.EventListenersSidebarPane._objectGroupName);delete this._lastRequestedNode;}
this._linkifier.reset();var body=this.bodyElement;body.removeChildren();this.sections=[];var node=this.node();if(!node){finishCallback();return;}
this._lastRequestedNode=node;node.eventListeners(WebInspector.EventListenersSidebarPane._objectGroupName,this._onEventListeners.bind(this,finishCallback));},_onEventListeners:function(finishCallback,eventListeners)
{if(!eventListeners){finishCallback();return;}
var body=this.bodyElement;var node=this.node();var selectedNodeOnly="selected"===WebInspector.settings.eventListenersFilter.get();var sectionNames=[];var sectionMap={};for(var i=0;i<eventListeners.length;++i){var eventListener=eventListeners[i];if(selectedNodeOnly&&(node.id!==eventListener.payload().nodeId))
continue;if(/^function _inspectorCommandLineAPI_logEvent\(/.test(eventListener.payload().handlerBody.toString()))
continue;var type=eventListener.payload().type;var section=sectionMap[type];if(!section){section=new WebInspector.EventListenersSection(type,node.id,this._linkifier);sectionMap[type]=section;sectionNames.push(type);this.sections.push(section);}
section.addListener(eventListener);}
if(sectionNames.length===0){body.createChild("div","info").textContent=WebInspector.UIString("No Event Listeners");}else{sectionNames.sort();for(var i=0;i<sectionNames.length;++i){var section=sectionMap[sectionNames[i]];body.appendChild(section.element);}}
finishCallback();},_changeSetting:function()
{var selectedOption=this.settingsSelectElement[this.settingsSelectElement.selectedIndex];WebInspector.settings.eventListenersFilter.set(selectedOption.value);this.update();},__proto__:WebInspector.ElementsSidebarPane.prototype}
WebInspector.EventListenersSection=function(title,nodeId,linkifier)
{this._nodeId=nodeId;this._linkifier=linkifier;WebInspector.Section.call(this,title);this.propertiesElement.remove();delete this.propertiesElement;delete this.propertiesTreeOutline;this._eventBars=this.element.createChild("div","event-bars");}
WebInspector.EventListenersSection.prototype={addListener:function(eventListener)
{var eventListenerBar=new WebInspector.EventListenerBar(eventListener,this._nodeId,this._linkifier);this._eventBars.appendChild(eventListenerBar.element);},__proto__:WebInspector.Section.prototype}
WebInspector.EventListenerBar=function(eventListener,nodeId,linkifier)
{var target=eventListener.target();WebInspector.ObjectPropertiesSection.call(this,target.runtimeModel.createRemoteObjectFromPrimitiveValue(""));this._runtimeModel=target.runtimeModel;this._eventListener=eventListener;this._nodeId=nodeId;this._setNodeTitle();this._setFunctionSubtitle(linkifier);this.editable=false;this.element.classList.add("event-bar");this.headerElement.classList.add("source-code");this.propertiesElement.classList.add("event-properties");}
WebInspector.EventListenerBar.prototype={update:function()
{function updateWithNodeObject(nodeObject)
{var properties=[];var payload=this._eventListener.payload();properties.push(this._runtimeModel.createRemotePropertyFromPrimitiveValue("type",payload.type));properties.push(this._runtimeModel.createRemotePropertyFromPrimitiveValue("useCapture",payload.useCapture));properties.push(this._runtimeModel.createRemotePropertyFromPrimitiveValue("isAttribute",payload.isAttribute));if(nodeObject)
properties.push(new WebInspector.RemoteObjectProperty("node",nodeObject));if(typeof payload.handler!=="undefined"){var remoteObject=this._runtimeModel.createRemoteObject(payload.handler);properties.push(new WebInspector.RemoteObjectProperty("handler",remoteObject));}
properties.push(this._runtimeModel.createRemotePropertyFromPrimitiveValue("listenerBody",payload.handlerBody));if(payload.sourceName)
properties.push(this._runtimeModel.createRemotePropertyFromPrimitiveValue("sourceName",payload.sourceName));properties.push(this._runtimeModel.createRemotePropertyFromPrimitiveValue("lineNumber",payload.location.lineNumber+1));this.updateProperties(properties);}
this._eventListener.node().resolveToObject(WebInspector.EventListenersSidebarPane._objectGroupName,updateWithNodeObject.bind(this));},_setNodeTitle:function()
{var node=this._eventListener.node();if(!node)
return;if(node.nodeType()===Node.DOCUMENT_NODE){this.titleElement.textContent="document";return;}
if(node.id===this._nodeId){this.titleElement.textContent=WebInspector.DOMPresentationUtils.simpleSelector(node);return;}
this.titleElement.removeChildren();this.titleElement.appendChild(WebInspector.DOMPresentationUtils.linkifyNodeReference(node));},_setFunctionSubtitle:function(linkifier)
{this.subtitleElement.removeChildren();var link=linkifier.linkifyRawLocation(this._eventListener.location(),this._eventListener.sourceName());this.subtitleElement.appendChild(link);},__proto__:WebInspector.ObjectPropertiesSection.prototype};WebInspector.MetricsSidebarPane=function()
{WebInspector.ElementsSidebarPane.call(this,WebInspector.UIString("Metrics"));}
WebInspector.MetricsSidebarPane.prototype={setNode:function(node)
{WebInspector.ElementsSidebarPane.prototype.setNode.call(this,node);this._updateTarget(node.target());},_updateTarget:function(target)
{if(this._target===target)
return;if(this._target){this._target.cssModel.removeEventListener(WebInspector.CSSStyleModel.Events.StyleSheetAdded,this.update,this);this._target.cssModel.removeEventListener(WebInspector.CSSStyleModel.Events.StyleSheetRemoved,this.update,this);this._target.cssModel.removeEventListener(WebInspector.CSSStyleModel.Events.StyleSheetChanged,this.update,this);this._target.cssModel.removeEventListener(WebInspector.CSSStyleModel.Events.MediaQueryResultChanged,this.update,this);this._target.domModel.removeEventListener(WebInspector.DOMModel.Events.AttrModified,this._attributesUpdated,this);this._target.domModel.removeEventListener(WebInspector.DOMModel.Events.AttrRemoved,this._attributesUpdated,this);this._target.resourceTreeModel.removeEventListener(WebInspector.ResourceTreeModel.EventTypes.FrameResized,this.update,this);}
this._target=target;this._target.cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetAdded,this.update,this);this._target.cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetRemoved,this.update,this);this._target.cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetChanged,this.update,this);this._target.cssModel.addEventListener(WebInspector.CSSStyleModel.Events.MediaQueryResultChanged,this.update,this);this._target.domModel.addEventListener(WebInspector.DOMModel.Events.AttrModified,this._attributesUpdated,this);this._target.domModel.addEventListener(WebInspector.DOMModel.Events.AttrRemoved,this._attributesUpdated,this);this._target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.FrameResized,this.update,this);},doUpdate:function(finishedCallback)
{if(this._isEditingMetrics){finishedCallback();return;}
var node=this.node();if(!node||node.nodeType()!==Node.ELEMENT_NODE){this.bodyElement.removeChildren();finishedCallback();return;}
function callback(style)
{if(!style||this.node()!==node)
return;this._updateMetrics(style);}
this._target.cssModel.getComputedStyleAsync(node.id,callback.bind(this));function inlineStyleCallback(style)
{if(style&&this.node()===node)
this.inlineStyle=style;finishedCallback();}
this._target.cssModel.getInlineStylesAsync(node.id,inlineStyleCallback.bind(this));},_attributesUpdated:function(event)
{if(this.node()!==event.data.node)
return;this.update();},_getPropertyValueAsPx:function(style,propertyName)
{return Number(style.getPropertyValue(propertyName).replace(/px$/,"")||0);},_getBox:function(computedStyle,componentName)
{var suffix=componentName==="border"?"-width":"";var left=this._getPropertyValueAsPx(computedStyle,componentName+"-left"+suffix);var top=this._getPropertyValueAsPx(computedStyle,componentName+"-top"+suffix);var right=this._getPropertyValueAsPx(computedStyle,componentName+"-right"+suffix);var bottom=this._getPropertyValueAsPx(computedStyle,componentName+"-bottom"+suffix);return{left:left,top:top,right:right,bottom:bottom};},_highlightDOMNode:function(showHighlight,mode,event)
{event.consume();if(showHighlight&&this.node()){if(this._highlightMode===mode)
return;this._highlightMode=mode;this.node().highlight(mode);}else{delete this._highlightMode;this._target.domModel.hideDOMNodeHighlight();}
for(var i=0;this._boxElements&&i<this._boxElements.length;++i){var element=this._boxElements[i];if(!this.node()||mode==="all"||element._name===mode)
element.style.backgroundColor=element._backgroundColor;else
element.style.backgroundColor="";}},_updateMetrics:function(style)
{var metricsElement=createElement("div");metricsElement.className="metrics";var self=this;function createBoxPartElement(style,name,side,suffix)
{var propertyName=(name!=="position"?name+"-":"")+side+suffix;var value=style.getPropertyValue(propertyName);if(value===""||(name!=="position"&&value==="0px"))
value="\u2012";else if(name==="position"&&value==="auto")
value="\u2012";value=value.replace(/px$/,"");value=Number.toFixedIfFloating(value);var element=createElement("div");element.className=side;element.textContent=value;element.addEventListener("dblclick",this.startEditing.bind(this,element,name,propertyName,style),false);return element;}
function getContentAreaWidthPx(style)
{var width=style.getPropertyValue("width").replace(/px$/,"");if(!isNaN(width)&&style.getPropertyValue("box-sizing")==="border-box"){var borderBox=self._getBox(style,"border");var paddingBox=self._getBox(style,"padding");width=width-borderBox.left-borderBox.right-paddingBox.left-paddingBox.right;}
return Number.toFixedIfFloating(width);}
function getContentAreaHeightPx(style)
{var height=style.getPropertyValue("height").replace(/px$/,"");if(!isNaN(height)&&style.getPropertyValue("box-sizing")==="border-box"){var borderBox=self._getBox(style,"border");var paddingBox=self._getBox(style,"padding");height=height-borderBox.top-borderBox.bottom-paddingBox.top-paddingBox.bottom;}
return Number.toFixedIfFloating(height);}
var noMarginDisplayType={"table-cell":true,"table-column":true,"table-column-group":true,"table-footer-group":true,"table-header-group":true,"table-row":true,"table-row-group":true};var noPaddingDisplayType={"table-column":true,"table-column-group":true,"table-footer-group":true,"table-header-group":true,"table-row":true,"table-row-group":true};var noPositionType={"static":true};var boxes=["content","padding","border","margin","position"];var boxColors=[WebInspector.Color.PageHighlight.Content,WebInspector.Color.PageHighlight.Padding,WebInspector.Color.PageHighlight.Border,WebInspector.Color.PageHighlight.Margin,WebInspector.Color.fromRGBA([0,0,0,0])];var boxLabels=[WebInspector.UIString("content"),WebInspector.UIString("padding"),WebInspector.UIString("border"),WebInspector.UIString("margin"),WebInspector.UIString("position")];var previousBox=null;this._boxElements=[];for(var i=0;i<boxes.length;++i){var name=boxes[i];if(name==="margin"&&noMarginDisplayType[style.getPropertyValue("display")])
continue;if(name==="padding"&&noPaddingDisplayType[style.getPropertyValue("display")])
continue;if(name==="position"&&noPositionType[style.getPropertyValue("position")])
continue;var boxElement=createElement("div");boxElement.className=name;boxElement._backgroundColor=boxColors[i].asString(WebInspector.Color.Format.RGBA);boxElement._name=name;boxElement.style.backgroundColor=boxElement._backgroundColor;boxElement.addEventListener("mouseover",this._highlightDOMNode.bind(this,true,name==="position"?"all":name),false);this._boxElements.push(boxElement);if(name==="content"){var widthElement=createElement("span");widthElement.textContent=getContentAreaWidthPx(style);widthElement.addEventListener("dblclick",this.startEditing.bind(this,widthElement,"width","width",style),false);var heightElement=createElement("span");heightElement.textContent=getContentAreaHeightPx(style);heightElement.addEventListener("dblclick",this.startEditing.bind(this,heightElement,"height","height",style),false);boxElement.appendChild(widthElement);boxElement.createTextChild(" \u00D7 ");boxElement.appendChild(heightElement);}else{var suffix=(name==="border"?"-width":"");var labelElement=createElement("div");labelElement.className="label";labelElement.textContent=boxLabels[i];boxElement.appendChild(labelElement);boxElement.appendChild(createBoxPartElement.call(this,style,name,"top",suffix));boxElement.appendChild(createElement("br"));boxElement.appendChild(createBoxPartElement.call(this,style,name,"left",suffix));if(previousBox)
boxElement.appendChild(previousBox);boxElement.appendChild(createBoxPartElement.call(this,style,name,"right",suffix));boxElement.appendChild(createElement("br"));boxElement.appendChild(createBoxPartElement.call(this,style,name,"bottom",suffix));}
previousBox=boxElement;}
metricsElement.appendChild(previousBox);metricsElement.addEventListener("mouseover",this._highlightDOMNode.bind(this,false,"all"),false);this.bodyElement.removeChildren();this.bodyElement.appendChild(metricsElement);},startEditing:function(targetElement,box,styleProperty,computedStyle)
{if(WebInspector.isBeingEdited(targetElement))
return;var context={box:box,styleProperty:styleProperty,computedStyle:computedStyle};var boundKeyDown=this._handleKeyDown.bind(this,context,styleProperty);context.keyDownHandler=boundKeyDown;targetElement.addEventListener("keydown",boundKeyDown,false);this._isEditingMetrics=true;var config=new WebInspector.InplaceEditor.Config(this.editingCommitted.bind(this),this.editingCancelled.bind(this),context);WebInspector.InplaceEditor.startEditing(targetElement,config);targetElement.window().getSelection().setBaseAndExtent(targetElement,0,targetElement,1);},_handleKeyDown:function(context,styleProperty,event)
{var element=event.currentTarget;function finishHandler(originalValue,replacementString)
{this._applyUserInput(element,replacementString,originalValue,context,false);}
function customNumberHandler(prefix,number,suffix)
{if(styleProperty!=="margin"&&number<0)
number=0;return prefix+number+suffix;}
WebInspector.handleElementValueModifications(event,element,finishHandler.bind(this),undefined,customNumberHandler);},editingEnded:function(element,context)
{delete this.originalPropertyData;delete this.previousPropertyDataCandidate;element.removeEventListener("keydown",context.keyDownHandler,false);delete this._isEditingMetrics;},editingCancelled:function(element,context)
{if("originalPropertyData"in this&&this.inlineStyle){if(!this.originalPropertyData){var pastLastSourcePropertyIndex=this.inlineStyle.pastLastSourcePropertyIndex();if(pastLastSourcePropertyIndex)
this.inlineStyle.allProperties[pastLastSourcePropertyIndex-1].setText("",false);}else
this.inlineStyle.allProperties[this.originalPropertyData.index].setText(this.originalPropertyData.propertyText,false);}
this.editingEnded(element,context);this.update();},_applyUserInput:function(element,userInput,previousContent,context,commitEditor)
{if(!this.inlineStyle){return this.editingCancelled(element,context);}
if(commitEditor&&userInput===previousContent)
return this.editingCancelled(element,context);if(context.box!=="position"&&(!userInput||userInput==="\u2012"))
userInput="0px";else if(context.box==="position"&&(!userInput||userInput==="\u2012"))
userInput="auto";userInput=userInput.toLowerCase();if(/^\d+$/.test(userInput))
userInput+="px";var styleProperty=context.styleProperty;var computedStyle=context.computedStyle;if(computedStyle.getPropertyValue("box-sizing")==="border-box"&&(styleProperty==="width"||styleProperty==="height")){if(!userInput.match(/px$/)){WebInspector.console.error("For elements with box-sizing: border-box, only absolute content area dimensions can be applied");return;}
var borderBox=this._getBox(computedStyle,"border");var paddingBox=this._getBox(computedStyle,"padding");var userValuePx=Number(userInput.replace(/px$/,""));if(isNaN(userValuePx))
return;if(styleProperty==="width")
userValuePx+=borderBox.left+borderBox.right+paddingBox.left+paddingBox.right;else
userValuePx+=borderBox.top+borderBox.bottom+paddingBox.top+paddingBox.bottom;userInput=userValuePx+"px";}
this.previousPropertyDataCandidate=null;var allProperties=this.inlineStyle.allProperties;for(var i=0;i<allProperties.length;++i){var property=allProperties[i];if(property.name!==context.styleProperty||property.inactive)
continue;this.previousPropertyDataCandidate=property;property.setValue(userInput,commitEditor,true,callback.bind(this));return;}
this.inlineStyle.appendProperty(context.styleProperty,userInput,callback.bind(this));function callback(style)
{if(!style)
return;this.inlineStyle=style;if(!("originalPropertyData"in this))
this.originalPropertyData=this.previousPropertyDataCandidate;if(typeof this._highlightMode!=="undefined")
this._node.highlight(this._highlightMode);if(commitEditor)
this.update();}},editingCommitted:function(element,userInput,previousContent,context)
{this.editingEnded(element,context);this._applyUserInput(element,userInput,previousContent,context,true);},__proto__:WebInspector.ElementsSidebarPane.prototype};WebInspector.PlatformFontsSidebarPane=function()
{WebInspector.ElementsSidebarPane.call(this,WebInspector.UIString("Fonts"));this.element.classList.add("platform-fonts");this._sectionTitle=createElementWithClass("div","sidebar-separator");this.element.insertBefore(this._sectionTitle,this.bodyElement);this._sectionTitle.textContent=WebInspector.UIString("Rendered Fonts");this._fontStatsSection=this.bodyElement.createChild("div","stats-section");}
WebInspector.PlatformFontsSidebarPane.prototype={setNode:function(node)
{WebInspector.ElementsSidebarPane.prototype.setNode.call(this,node);this._updateTarget(node.target());},_updateTarget:function(target)
{if(this._target===target)
return;if(this._target){this._target.cssModel.removeEventListener(WebInspector.CSSStyleModel.Events.StyleSheetAdded,this.update,this);this._target.cssModel.removeEventListener(WebInspector.CSSStyleModel.Events.StyleSheetRemoved,this.update,this);this._target.cssModel.removeEventListener(WebInspector.CSSStyleModel.Events.StyleSheetChanged,this.update,this);this._target.cssModel.removeEventListener(WebInspector.CSSStyleModel.Events.MediaQueryResultChanged,this.update,this);this._target.domModel.removeEventListener(WebInspector.DOMModel.Events.AttrModified,this.update,this);this._target.domModel.removeEventListener(WebInspector.DOMModel.Events.AttrRemoved,this.update,this);this._target.domModel.removeEventListener(WebInspector.DOMModel.Events.CharacterDataModified,this.update,this);}
this._target=target;this._target.cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetAdded,this.update,this);this._target.cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetRemoved,this.update,this);this._target.cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetChanged,this.update,this);this._target.cssModel.addEventListener(WebInspector.CSSStyleModel.Events.MediaQueryResultChanged,this.update,this);this._target.domModel.addEventListener(WebInspector.DOMModel.Events.AttrModified,this.update,this);this._target.domModel.addEventListener(WebInspector.DOMModel.Events.AttrRemoved,this.update,this);this._target.domModel.addEventListener(WebInspector.DOMModel.Events.CharacterDataModified,this.update,this);},doUpdate:function(finishedCallback)
{if(!this.node())
return;this._target.cssModel.getPlatformFontsForNode(this.node().id,this._refreshUI.bind(this,(this.node()),finishedCallback));},_refreshUI:function(node,finishedCallback,cssFamilyName,platformFonts)
{if(this.node()!==node){finishedCallback();return;}
this._fontStatsSection.removeChildren();var isEmptySection=!platformFonts||!platformFonts.length;this._sectionTitle.classList.toggle("hidden",isEmptySection);if(isEmptySection){finishedCallback();return;}
platformFonts.sort(function(a,b){return b.glyphCount-a.glyphCount;});for(var i=0;i<platformFonts.length;++i){var fontStatElement=this._fontStatsSection.createChild("div","font-stats-item");var fontNameElement=fontStatElement.createChild("span","font-name");fontNameElement.textContent=platformFonts[i].familyName;var fontDelimeterElement=fontStatElement.createChild("span","delimeter");fontDelimeterElement.textContent="\u2014";var fontUsageElement=fontStatElement.createChild("span","font-usage");var usage=platformFonts[i].glyphCount;fontUsageElement.textContent=usage===1?WebInspector.UIString("%d glyph",usage):WebInspector.UIString("%d glyphs",usage);}
finishedCallback();},__proto__:WebInspector.ElementsSidebarPane.prototype};WebInspector.PropertiesSidebarPane=function()
{WebInspector.ElementsSidebarPane.call(this,WebInspector.UIString("Properties"));WebInspector.targetManager.addModelListener(WebInspector.DOMModel,WebInspector.DOMModel.Events.AttrModified,this._onNodeChange,this);WebInspector.targetManager.addModelListener(WebInspector.DOMModel,WebInspector.DOMModel.Events.AttrRemoved,this._onNodeChange,this);WebInspector.targetManager.addModelListener(WebInspector.DOMModel,WebInspector.DOMModel.Events.CharacterDataModified,this._onNodeChange,this);WebInspector.targetManager.addModelListener(WebInspector.DOMModel,WebInspector.DOMModel.Events.ChildNodeCountUpdated,this._onNodeChange,this);}
WebInspector.PropertiesSidebarPane._objectGroupName="properties-sidebar-pane";WebInspector.PropertiesSidebarPane.prototype={doUpdate:function(finishCallback)
{if(this._lastRequestedNode){this._lastRequestedNode.target().runtimeAgent().releaseObjectGroup(WebInspector.PropertiesSidebarPane._objectGroupName);delete this._lastRequestedNode;}
var node=this.node();if(!node){this.bodyElement.removeChildren();this.sections=[];finishCallback();return;}
this._lastRequestedNode=node;node.resolveToObject(WebInspector.PropertiesSidebarPane._objectGroupName,nodeResolved.bind(this));function nodeResolved(object)
{if(!object){finishCallback();return;}
function protoList()
{var proto=this;var result={__proto__:null};var counter=1;while(proto){result[counter++]=proto;proto=proto.__proto__;}
return result;}
object.callFunction(protoList,undefined,nodePrototypesReady.bind(this));object.release();}
function nodePrototypesReady(object,wasThrown)
{if(!object||wasThrown){finishCallback();return;}
object.getOwnProperties(fillSection.bind(this));object.release();}
function fillSection(prototypes)
{if(!prototypes){finishCallback();return;}
var expanded=[];var sections=this.sections||[];for(var i=0;i<sections.length;++i)
expanded.push(sections[i].expanded);var body=this.bodyElement;body.removeChildren();this.sections=[];for(var i=0;i<prototypes.length;++i){if(!parseInt(prototypes[i].name,10))
continue;var prototype=prototypes[i].value;var title=prototype.description;title=title.replace(/Prototype$/,"");var section=new WebInspector.ObjectPropertiesSection(prototype,title);this.sections.push(section);body.appendChild(section.element);if(expanded[this.sections.length-1])
section.expand();}
finishCallback();}},_onNodeChange:function(event)
{if(!this.node())
return;var data=event.data;var node=(data instanceof WebInspector.DOMNode?data:data.node);if(this.node()!==node)
return;this.update();},__proto__:WebInspector.ElementsSidebarPane.prototype};WebInspector.StylesSectionModel=function(cascade,rule,style,customSelectorText,inheritedFromNode)
{this._cascade=cascade;this._rule=rule;this._style=style;this._customSelectorText=customSelectorText;this._isAttribute=false;this._editable=!!(this._style&&this._style.styleSheetId);this._inheritedFromNode=inheritedFromNode||null;}
WebInspector.StylesSectionModel.prototype={cascade:function()
{return this._cascade;},hasMatchingSelectors:function()
{return this.rule()?this.rule().matchingSelectors.length>0&&this.mediaMatches():true;},mediaMatches:function()
{var media=this.media();for(var i=0;media&&i<media.length;++i){if(!media[i].active())
return false;}
return true;},inherited:function()
{return!!this._inheritedFromNode;},parentNode:function()
{return this._inheritedFromNode;},selectorText:function()
{if(this._customSelectorText)
return this._customSelectorText;return this.rule()?this.rule().selectorText:"";},editable:function()
{return this._editable;},setEditable:function(editable)
{this._editable=editable;},style:function()
{return this._style;},rule:function()
{return this._rule;},media:function()
{return this.rule()?this.rule().media:null;},isAttribute:function()
{return this._isAttribute;},setIsAttribute:function(isAttribute)
{this._isAttribute=isAttribute;},updateRule:function(rule)
{this._rule=rule;this._style=rule.style;this._cascade._resetUsedProperties();},updateStyleDeclaration:function(style)
{this._style=style;if(this._rule){style.parentRule=this._rule;this._rule.style=style;}
this._cascade._resetUsedProperties();},usedProperties:function()
{return this._cascade._usedPropertiesForModel(this);},isPropertyOverloaded:function(propertyName,isShorthand)
{if(!this.hasMatchingSelectors())
return false;if(this.inherited()&&!WebInspector.CSSMetadata.isPropertyInherited(propertyName)){return false;}
var canonicalName=WebInspector.CSSMetadata.canonicalPropertyName(propertyName);var used=this.usedProperties().has(canonicalName);if(used||!isShorthand)
return!used;var longhandProperties=this.style().longhandProperties(propertyName);for(var j=0;j<longhandProperties.length;++j){var individualProperty=longhandProperties[j];var canonicalPropertyName=WebInspector.CSSMetadata.canonicalPropertyName(individualProperty.name);if(this.usedProperties().has(canonicalPropertyName))
return false;}
return true;}}
WebInspector.SectionCascade=function()
{this._models=[];this._resetUsedProperties();}
WebInspector.SectionCascade.prototype={sectionModels:function()
{return this._models;},appendModelFromRule:function(rule,inheritedFromNode)
{return this._insertModel(new WebInspector.StylesSectionModel(this,rule,rule.style,"",inheritedFromNode));},insertModelFromRule:function(rule,insertAfterStyleRule)
{return this._insertModel(new WebInspector.StylesSectionModel(this,rule,rule.style,"",null),insertAfterStyleRule);},appendModelFromStyle:function(style,selectorText,inheritedFromNode)
{return this._insertModel(new WebInspector.StylesSectionModel(this,null,style,selectorText,inheritedFromNode));},allUsedProperties:function()
{this._recomputeUsedPropertiesIfNeeded();return this._allUsedProperties;},_insertModel:function(model,insertAfter)
{if(insertAfter){var index=this._models.indexOf(insertAfter);console.assert(index!==-1,"The insertAfter anchor could not be found in cascade");this._models.splice(index+1,0,model);}else{this._models.push(model);}
this._resetUsedProperties();return model;},_recomputeUsedPropertiesIfNeeded:function()
{if(this._usedPropertiesPerModel.size>0)
return;var usedProperties=WebInspector.SectionCascade._computeUsedProperties(this._models,this._allUsedProperties);for(var i=0;i<usedProperties.length;++i)
this._usedPropertiesPerModel.set(this._models[i],usedProperties[i]);},_resetUsedProperties:function()
{this._allUsedProperties=new Set();this._usedPropertiesPerModel=new Map();},_usedPropertiesForModel:function(model)
{this._recomputeUsedPropertiesIfNeeded();return this._usedPropertiesPerModel.get(model);}}
WebInspector.SectionCascade._computeUsedProperties=function(styleRules,allUsedProperties)
{var foundImportantProperties=new Set();var propertyToEffectiveRule=new Map();var inheritedPropertyToNode=new Map();var stylesUsedProperties=[];for(var i=0;i<styleRules.length;++i){var styleRule=styleRules[i];var styleRuleUsedProperties=new Set();stylesUsedProperties.push(styleRuleUsedProperties);if(!styleRule.hasMatchingSelectors())
continue;var style=styleRule.style();var allProperties=style.allProperties;for(var j=0;j<allProperties.length;++j){var property=allProperties[j];if(!property.isLive||!property.parsedOk)
continue;if(styleRule.inherited()&&!WebInspector.CSSMetadata.isPropertyInherited(property.name))
continue;var canonicalName=WebInspector.CSSMetadata.canonicalPropertyName(property.name);if(foundImportantProperties.has(canonicalName))
continue;if(!property.important&&allUsedProperties.has(canonicalName))
continue;var isKnownProperty=propertyToEffectiveRule.has(canonicalName);var parentNode=styleRule.parentNode();if(!isKnownProperty&&parentNode&&!inheritedPropertyToNode.has(canonicalName))
inheritedPropertyToNode.set(canonicalName,parentNode);if(property.important){if(styleRule.inherited()&&isKnownProperty&&styleRule.parentNode()!==inheritedPropertyToNode.get(canonicalName))
continue;foundImportantProperties.add(canonicalName);if(isKnownProperty)
propertyToEffectiveRule.get(canonicalName).delete(canonicalName);}
styleRuleUsedProperties.add(canonicalName);allUsedProperties.add(canonicalName);propertyToEffectiveRule.set(canonicalName,styleRuleUsedProperties);}}
return stylesUsedProperties;};WebInspector.StylesSidebarPane=function(computedStylePane,setPseudoClassCallback)
{WebInspector.SidebarPane.call(this,WebInspector.UIString("Styles"));this._animationsControlButton=createElement("button");this._animationsControlButton.className="pane-title-button animations-controls";this._animationsControlButton.title=WebInspector.UIString("Animations Controls");this._animationsControlButton.addEventListener("click",this._toggleAnimationsControlPane.bind(this),false);this.titleElement.appendChild(this._animationsControlButton);this._elementStateButton=createElement("button");this._elementStateButton.className="pane-title-button element-state";this._elementStateButton.title=WebInspector.UIString("Toggle Element State");this._elementStateButton.addEventListener("click",this._toggleElementStatePane.bind(this),false);this.titleElement.appendChild(this._elementStateButton);var addButton=createElement("button");addButton.className="pane-title-button add";addButton.id="add-style-button-test-id";addButton.title=WebInspector.UIString("New Style Rule");addButton.addEventListener("click",this._createNewRuleInViaInspectorStyleSheet.bind(this),false);this.titleElement.appendChild(addButton);addButton.createChild("div","long-click-glyph fill");this._addButtonLongClickController=new WebInspector.LongClickController(addButton);this._addButtonLongClickController.addEventListener(WebInspector.LongClickController.Events.LongClick,this._onAddButtonLongClick.bind(this));this._addButtonLongClickController.enable();this._computedStylePane=computedStylePane;computedStylePane.setHostingPane(this);this._setPseudoClassCallback=setPseudoClassCallback;this.element.addEventListener("contextmenu",this._contextMenuEventFired.bind(this),true);WebInspector.settings.colorFormat.addChangeListener(this._colorFormatSettingChanged.bind(this));WebInspector.settings.showUserAgentStyles.addChangeListener(this._showUserAgentStylesSettingChanged.bind(this));this._createElementStatePane();this.bodyElement.appendChild(this._elementStatePane);this._createAnimationsControlPane();this.bodyElement.appendChild(this._animationsControlPane);this._sectionsContainer=createElement("div");this.bodyElement.appendChild(this._sectionsContainer);this._spectrumHelper=new WebInspector.SpectrumPopupHelper();this._linkifier=new WebInspector.Linkifier(new WebInspector.Linkifier.DefaultCSSFormatter());this.element.classList.add("styles-pane");this.element.classList.toggle("show-user-styles",WebInspector.settings.showUserAgentStyles.get());this.element.addEventListener("mousemove",this._mouseMovedOverElement.bind(this),false);this._keyDownBound=this._keyDown.bind(this);this._keyUpBound=this._keyUp.bind(this);}
WebInspector.StylesSidebarPane.PseudoIdNames=["","first-line","first-letter","before","after","backdrop","selection","","-webkit-scrollbar","-webkit-scrollbar-thumb","-webkit-scrollbar-button","-webkit-scrollbar-track","-webkit-scrollbar-track-piece","-webkit-scrollbar-corner","-webkit-resizer"];WebInspector.StylesSidebarPane._colorRegex=/((?:rgb|hsl)a?\([^)]+\)|#[0-9a-fA-F]{6}|#[0-9a-fA-F]{3}|\b\w+\b(?!-))/g;WebInspector.StylesSidebarPane.Events={SelectorEditingStarted:"SelectorEditingStarted",SelectorEditingEnded:"SelectorEditingEnded"};WebInspector.StylesSidebarPane.createExclamationMark=function(property)
{var exclamationElement=createElement("div");exclamationElement.className="exclamation-mark"+(WebInspector.StylesSidebarPane._ignoreErrorsForProperty(property)?"":" warning-icon-small");exclamationElement.title=WebInspector.CSSMetadata.cssPropertiesMetainfo.keySet()[property.name.toLowerCase()]?WebInspector.UIString("Invalid property value."):WebInspector.UIString("Unknown property name.");return exclamationElement;}
WebInspector.StylesSidebarPane._colorFormat=function(color)
{const cf=WebInspector.Color.Format;var format;var formatSetting=WebInspector.settings.colorFormat.get();if(formatSetting===cf.Original)
format=cf.Original;else if(formatSetting===cf.RGB)
format=(color.hasAlpha()?cf.RGBA:cf.RGB);else if(formatSetting===cf.HSL)
format=(color.hasAlpha()?cf.HSLA:cf.HSL);else if(!color.hasAlpha())
format=(color.canBeShortHex()?cf.ShortHEX:cf.HEX);else
format=cf.RGBA;return format;}
WebInspector.StylesSidebarPane._ignoreErrorsForProperty=function(property){function hasUnknownVendorPrefix(string)
{return!string.startsWith("-webkit-")&&/^[-_][\w\d]+-\w/.test(string);}
var name=property.name.toLowerCase();if(name.charAt(0)==="_")
return true;if(name==="filter")
return true;if(name.startsWith("scrollbar-"))
return true;if(hasUnknownVendorPrefix(name))
return true;var value=property.value.toLowerCase();if(value.endsWith("\9"))
return true;if(hasUnknownVendorPrefix(value))
return true;return false;}
WebInspector.StylesSidebarPane.prototype={node:function()
{return this._node;},_onAddButtonLongClick:function(event)
{this._addButtonLongClickController.reset();var cssModel=this._target.cssModel;var headers=cssModel.styleSheetHeaders().filter(styleSheetResourceHeader);var contextMenuDescriptors=[];for(var i=0;i<headers.length;++i){var header=headers[i];var handler=this._createNewRuleInStyleSheet.bind(this,header);contextMenuDescriptors.push({text:WebInspector.displayNameForURL(header.resourceURL()),handler:handler});}
contextMenuDescriptors.sort(compareDescriptors);var contextMenu=new WebInspector.ContextMenu((event.data));for(var i=0;i<contextMenuDescriptors.length;++i){var descriptor=contextMenuDescriptors[i];contextMenu.appendItem(descriptor.text,descriptor.handler);}
if(!contextMenu.isEmpty())
contextMenu.appendSeparator();contextMenu.appendItem("inspector-stylesheet",this._createNewRuleInViaInspectorStyleSheet.bind(this));contextMenu.show();function compareDescriptors(descriptor1,descriptor2)
{return String.naturalOrderComparator(descriptor1.text,descriptor2.text);}
function styleSheetResourceHeader(header)
{return!header.isViaInspector()&&!header.isInline&&!!header.resourceURL();}},updateEditingSelectorForNode:function(node)
{var selectorText=WebInspector.DOMPresentationUtils.simpleSelector(node);if(!selectorText)
return;this._editingSelectorSection.setSelectorText(selectorText);},isEditingSelector:function()
{return!!this._editingSelectorSection;},_startEditingSelector:function(section)
{this._editingSelectorSection=section;this.dispatchEventToListeners(WebInspector.StylesSidebarPane.Events.SelectorEditingStarted);},_finishEditingSelector:function()
{delete this._editingSelectorSection;this.dispatchEventToListeners(WebInspector.StylesSidebarPane.Events.SelectorEditingEnded);},_styleSheetRuleEdited:function(editedRule,oldRange,newRange)
{if(!editedRule.styleSheetId)
return;for(var pseudoId in this.sections){var styleRuleSections=this.sections[pseudoId];for(var i=0;i<styleRuleSections.length;++i){var section=styleRuleSections[i];if(section.computedStyle)
continue;section._styleSheetRuleEdited(editedRule,oldRange,newRange);}}},_styleSheetMediaEdited:function(oldMedia,newMedia)
{if(!oldMedia.parentStyleSheetId)
return;for(var pseudoId in this.sections){var styleRuleSections=this.sections[pseudoId];for(var i=0;i<styleRuleSections.length;++i){var section=styleRuleSections[i];if(section.computedStyle)
continue;section._styleSheetMediaEdited(oldMedia,newMedia);}}},_contextMenuEventFired:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendApplicableItems((event.target));contextMenu.show();},setFilterBoxContainer:function(matchedStylesElement)
{matchedStylesElement.appendChild(this._createCSSFilterControl());},_createCSSFilterControl:function()
{var filterInput=WebInspector.StylesSidebarPane._createPropertyFilterElement(WebInspector.UIString("Find in Styles"),searchHandler.bind(this));function searchHandler(regex)
{this._filterRegex=regex;this._updateFilter();}
return filterInput;},get _forcedPseudoClasses()
{return this._node?(this._node.getUserProperty(WebInspector.CSSStyleModel.PseudoStatePropertyName)||undefined):undefined;},_updateForcedPseudoStateInputs:function()
{if(!this._node)
return;var hasPseudoType=!!this._node.pseudoType();this._elementStateButton.classList.toggle("hidden",hasPseudoType);this._elementStatePane.classList.toggle("expanded",!hasPseudoType&&this._elementStateButton.classList.contains("toggled"));var nodePseudoState=this._forcedPseudoClasses;if(!nodePseudoState)
nodePseudoState=[];var inputs=this._elementStatePane.inputs;for(var i=0;i<inputs.length;++i)
inputs[i].checked=nodePseudoState.indexOf(inputs[i].state)>=0;},setNode:function(node)
{this._spectrumHelper.hide();this._discardElementUnderMouse();if(node&&node.nodeType()===Node.TEXT_NODE&&node.parentNode)
node=node.parentNode;if(node&&node.nodeType()!==Node.ELEMENT_NODE)
node=null;this._node=node;if(node)
this._updateTarget(node.target());this._computedStylePane.setNode(node);this._resetCache();this._scheduleUpdate();},_scheduleUpdate:function()
{if(!this.isShowing()&&!this._computedStylePane.isShowing()){this._updateWhenVisible=true;return;}
this._rebuildUpdate();},_updateTarget:function(target)
{if(this._target===target)
return;if(this._target){this._target.cssModel.removeEventListener(WebInspector.CSSStyleModel.Events.StyleSheetAdded,this._styleSheetOrMediaQueryResultChanged,this);this._target.cssModel.removeEventListener(WebInspector.CSSStyleModel.Events.StyleSheetRemoved,this._styleSheetOrMediaQueryResultChanged,this);this._target.cssModel.removeEventListener(WebInspector.CSSStyleModel.Events.StyleSheetChanged,this._styleSheetOrMediaQueryResultChanged,this);this._target.cssModel.removeEventListener(WebInspector.CSSStyleModel.Events.MediaQueryResultChanged,this._styleSheetOrMediaQueryResultChanged,this);this._target.domModel.removeEventListener(WebInspector.DOMModel.Events.AttrModified,this._attributeChanged,this);this._target.domModel.removeEventListener(WebInspector.DOMModel.Events.AttrRemoved,this._attributeChanged,this);this._target.resourceTreeModel.removeEventListener(WebInspector.ResourceTreeModel.EventTypes.FrameResized,this._frameResized,this);}
this._target=target;this._target.cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetAdded,this._styleSheetOrMediaQueryResultChanged,this);this._target.cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetRemoved,this._styleSheetOrMediaQueryResultChanged,this);this._target.cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetChanged,this._styleSheetOrMediaQueryResultChanged,this);this._target.cssModel.addEventListener(WebInspector.CSSStyleModel.Events.MediaQueryResultChanged,this._styleSheetOrMediaQueryResultChanged,this);this._target.domModel.addEventListener(WebInspector.DOMModel.Events.AttrModified,this._attributeChanged,this);this._target.domModel.addEventListener(WebInspector.DOMModel.Events.AttrRemoved,this._attributeChanged,this);this._target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.FrameResized,this._frameResized,this);},_refreshUpdate:function(editedSection)
{var node=this._validateNode();if(!node)
return;this._innerRefreshUpdate(node,editedSection);if(this._filterRegex)
this._updateFilter();},_rebuildUpdate:function()
{this._updateForcedPseudoStateInputs();if(this._rebuildUpdateInProgress){this._lastNodeForInnerRebuild=this.node();return;}
var node=this._validateNode();if(!node)
return;this._rebuildUpdateInProgress=true;this._fetchMatchedCascade().then(onStylesLoaded.bind(this));function onStylesLoaded(cascades)
{var lastRequestedRebuildNode=this._lastNodeForInnerRebuild||node;delete this._rebuildUpdateInProgress;delete this._lastNodeForInnerRebuild;if(node!==lastRequestedRebuildNode){this._rebuildUpdate();return;}
this._innerRebuildUpdate(cascades);}},_resetCache:function()
{delete this._matchedCascadePromise;this._resetComputedCache();},_resetComputedCache:function()
{delete this._computedCascadePromise;delete this._animationPropertiesPromise;},_fetchMatchedCascade:function()
{var node=this.node();if(!node)
return Promise.resolve((null));if(!this._matchedCascadePromise)
this._matchedCascadePromise=new Promise(this._getMatchedStylesForNode.bind(this,node)).then(buildMatchedCascades.bind(this,node));return this._matchedCascadePromise;function buildMatchedCascades(node,payload)
{if(node!==this.node()||!payload.fulfilled())
return null;return{matched:this._buildMatchedRulesSectionCascade(node,payload),pseudo:this._buildPseudoCascades(node,payload)};}},_fetchComputedCascade:function()
{var node=this.node();if(!node)
return Promise.resolve((null));if(!this._computedCascadePromise)
this._computedCascadePromise=new Promise(getComputedStyle.bind(null,node)).then(buildComputedCascade.bind(this,node));return this._computedCascadePromise;function getComputedStyle(node,resolve)
{node.target().cssModel.getComputedStyleAsync(node.id,resolve);}
function buildComputedCascade(node,styles)
{if(node!==this.node())
return null;if(!styles)
return null;var computedCascade=new WebInspector.SectionCascade();computedCascade.appendModelFromStyle(styles,"");return computedCascade;}},_fetchAnimationProperties:function()
{var node=this.node();if(!node)
return Promise.resolve(new Map());if(!this._animationPropertiesPromise)
this._animationPropertiesPromise=new Promise(this._getAnimationPropertiesForNode.bind(this,node)).then(onAnimationProperties.bind(this));return this._animationPropertiesPromise;function onAnimationProperties(properties)
{return this.node()!==node?new Map():properties;}},_getMatchedStylesForNode:function(node,callback)
{var target=node.target();target.cssModel.getInlineStylesAsync(node.id,inlineCallback);target.cssModel.getMatchedStylesAsync(node.id,false,false,matchedCallback);var payload=new WebInspector.StylesSidebarPane.MatchedRulesPayload();function inlineCallback(inlineStyle,attributesStyle)
{payload.inlineStyle=(inlineStyle);payload.attributesStyle=(attributesStyle);}
function matchedCallback(matchedResult)
{if(matchedResult){payload.matchedCSSRules=(matchedResult.matchedCSSRules);payload.pseudoElements=(matchedResult.pseudoElements);payload.inherited=(matchedResult.inherited);}
callback(payload);}},_getAnimationPropertiesForNode:function(node,callback)
{if(Runtime.experiments.isEnabled("animationInspection"))
node.target().animationModel.getAnimationPlayers(node.id,false,animationPlayersCallback);else
callback(new Map());function animationPlayersCallback(animationPlayers)
{var animationProperties=new Map();if(!animationPlayers)
return;for(var i=0;i<animationPlayers.length;i++){var player=animationPlayers[i];if(!player.source().keyframesRule())
continue;var animationCascade=new WebInspector.SectionCascade();var keyframes=player.source().keyframesRule().keyframes();for(var j=0;j<keyframes.length;j++)
animationCascade.appendModelFromStyle(keyframes[j].style(),"");for(var property of animationCascade.allUsedProperties())
animationProperties.set(property,player.name());}
callback(animationProperties);}},_validateNode:function()
{if(!this._node){this._sectionsContainer.removeChildren();this.sections={};return null;}
return this._node;},_styleSheetOrMediaQueryResultChanged:function()
{if(this._userOperation||this._isEditingStyle){this._resetComputedCache();return;}
this._resetCache();this._scheduleUpdate();},_frameResized:function()
{function refreshContents()
{this._styleSheetOrMediaQueryResultChanged();delete this._activeTimer;}
if(this._activeTimer)
clearTimeout(this._activeTimer);this._activeTimer=setTimeout(refreshContents.bind(this),100);},_attributeChanged:function(event)
{if(this._isEditingStyle||this._userOperation){this._resetComputedCache();return;}
if(!this._canAffectCurrentStyles(event.data.node))
return;this._resetCache();this._scheduleUpdate();},_canAffectCurrentStyles:function(node)
{return this._node&&(this._node===node||node.parentNode===this._node.parentNode||node.isAncestor(this._node));},_innerRefreshUpdate:function(node,editedSection)
{for(var pseudoId in this.sections){var sections=this.sections[pseudoId].filter(nonBlankSections);for(var section of sections)
section.update(section===editedSection);}
this._computedStylePane.update();this._nodeStylesUpdatedForTest(node,false);function nonBlankSections(section)
{return!section.isBlank;}},_innerRebuildUpdate:function(cascades)
{this._linkifier.reset();this._sectionsContainer.removeChildren();this.sections={};var node=this.node();if(!cascades||!node)
return;if(!!node.pseudoType())
this._appendTopPadding();this.sections[0]=this._rebuildSectionsForStyleRules(cascades.matched);this._computedStylePane.update();var pseudoIds=cascades.pseudo.keysArray().sort();for(var pseudoId of pseudoIds){this._appendSectionPseudoIdSeparator(pseudoId);this.sections[pseudoId]=this._rebuildSectionsForStyleRules(cascades.pseudo.get(pseudoId));}
if(this._filterRegex)
this._updateFilter();this._nodeStylesUpdatedForTest(node,true);this._updateAnimationsPlaybackRate();},_buildPseudoCascades:function(node,styles)
{var pseudoCascades=new Map();for(var i=0;i<styles.pseudoElements.length;++i){var pseudoElementCSSRules=styles.pseudoElements[i];var pseudoId=pseudoElementCSSRules.pseudoId;var pseudoElementCascade=new WebInspector.SectionCascade();for(var j=pseudoElementCSSRules.rules.length-1;j>=0;--j){var rule=pseudoElementCSSRules.rules[j];pseudoElementCascade.appendModelFromRule(rule);}
pseudoCascades.set(pseudoId,pseudoElementCascade);}
return pseudoCascades;},_nodeStylesUpdatedForTest:function(node,rebuild)
{},_buildMatchedRulesSectionCascade:function(node,styles)
{var cascade=new WebInspector.SectionCascade();function addAttributesStyle()
{if(!styles.attributesStyle)
return;var selectorText=node.nodeNameInCorrectCase()+"["+WebInspector.UIString("Attributes Style")+"]";cascade.appendModelFromStyle(styles.attributesStyle,selectorText);}
if(styles.inlineStyle&&node.nodeType()===Node.ELEMENT_NODE){var model=cascade.appendModelFromStyle(styles.inlineStyle,"element.style");model.setIsAttribute(true);}
var addedAttributesStyle;for(var i=styles.matchedCSSRules.length-1;i>=0;--i){var rule=styles.matchedCSSRules[i];if((rule.isInjected||rule.isUserAgent)&&!addedAttributesStyle){addedAttributesStyle=true;addAttributesStyle();}
cascade.appendModelFromRule(rule);}
if(!addedAttributesStyle)
addAttributesStyle();var parentNode=node.parentNode;for(var parentOrdinal=0;parentOrdinal<styles.inherited.length;++parentOrdinal){var parentStyles=styles.inherited[parentOrdinal];if(parentStyles.inlineStyle){if(this._containsInherited(parentStyles.inlineStyle)){var model=cascade.appendModelFromStyle(parentStyles.inlineStyle,WebInspector.UIString("Style Attribute"),parentNode);model.setIsAttribute(true);}}
for(var i=parentStyles.matchedCSSRules.length-1;i>=0;--i){var rulePayload=parentStyles.matchedCSSRules[i];if(!this._containsInherited(rulePayload.style))
continue;cascade.appendModelFromRule(rulePayload,parentNode);}
parentNode=parentNode.parentNode;}
return cascade;},_appendTopPadding:function()
{var separatorElement=createElement("div");separatorElement.className="styles-sidebar-placeholder";this._sectionsContainer.appendChild(separatorElement);},_appendSectionPseudoIdSeparator:function(pseudoId)
{var separatorElement=createElement("div");separatorElement.className="sidebar-separator";var pseudoName=WebInspector.StylesSidebarPane.PseudoIdNames[pseudoId];if(pseudoName)
separatorElement.textContent=WebInspector.UIString("Pseudo ::%s element",pseudoName);else
separatorElement.textContent=WebInspector.UIString("Pseudo element");this._sectionsContainer.appendChild(separatorElement);},_appendSectionInheritedNodeSeparator:function(node)
{var separatorElement=createElement("div");separatorElement.className="sidebar-separator";var link=WebInspector.DOMPresentationUtils.linkifyNodeReference(node);separatorElement.createTextChild(WebInspector.UIString("Inherited from")+" ");separatorElement.appendChild(link);this._sectionsContainer.appendChild(separatorElement);},_rebuildSectionsForStyleRules:function(cascade)
{var sections=[];var lastParentNode=null;for(var sectionModel of cascade.sectionModels()){var parentNode=sectionModel.parentNode();if(parentNode&&parentNode!==lastParentNode){lastParentNode=parentNode;this._appendSectionInheritedNodeSeparator(lastParentNode);}
var section=new WebInspector.StylePropertiesSection(this,sectionModel);section._markSelectorMatches();section.expand();this._sectionsContainer.appendChild(section.element);sections.push(section);}
return sections;},_containsInherited:function(style)
{var properties=style.allProperties;for(var i=0;i<properties.length;++i){var property=properties[i];if(property.isLive&&WebInspector.CSSMetadata.isPropertyInherited(property.name))
return true;}
return false;},_colorFormatSettingChanged:function(event)
{for(var pseudoId in this.sections){var sections=this.sections[pseudoId];for(var i=0;i<sections.length;++i)
sections[i].update(true);}},_createNewRuleInViaInspectorStyleSheet:function(event)
{var cssModel=this._target.cssModel;cssModel.requestViaInspectorStylesheet(this._node,this._createNewRuleInStyleSheet.bind(this));},_createNewRuleInStyleSheet:function(styleSheetHeader)
{if(!styleSheetHeader)
return;styleSheetHeader.requestContent(onStyleSheetContent.bind(this,styleSheetHeader.id));function onStyleSheetContent(styleSheetId,text)
{var lines=text.split("\n");var range=WebInspector.TextRange.createFromLocation(lines.length-1,lines[lines.length-1].length);this._addBlankSection(this.sections[0][0],styleSheetId,range);}},_addBlankSection:function(insertAfterSection,styleSheetId,ruleLocation)
{this.expand();var blankSection=new WebInspector.BlankStylePropertiesSection(this,this._node?WebInspector.DOMPresentationUtils.simpleSelector(this._node):"",styleSheetId,ruleLocation,insertAfterSection.styleRule);this._sectionsContainer.insertBefore(blankSection.element,insertAfterSection.element.nextSibling);var index=this.sections[0].indexOf(insertAfterSection);this.sections[0].splice(index+1,0,blankSection);blankSection.startEditingSelector();},removeSection:function(section)
{for(var pseudoId in this.sections){var sections=this.sections[pseudoId];var index=sections.indexOf(section);if(index===-1)
continue;sections.splice(index,1);section.element.remove();}},_toggleElementStatePane:function(event)
{event.consume();var buttonToggled=!this._elementStateButton.classList.contains("toggled");if(buttonToggled)
this.expand();this._elementStateButton.classList.toggle("toggled",buttonToggled);this._elementStatePane.classList.toggle("expanded",buttonToggled);this._animationsControlButton.classList.remove("toggled");this._animationsControlPane.classList.remove("expanded");},_createElementStatePane:function()
{this._elementStatePane=createElement("div");this._elementStatePane.className="styles-element-state-pane source-code";var table=createElement("table");var inputs=[];this._elementStatePane.inputs=inputs;function clickListener(event)
{var node=this._validateNode();if(!node)
return;this._setPseudoClassCallback(node,event.target.state,event.target.checked);}
function createCheckbox(state)
{var td=createElement("td");var label=createCheckboxLabel(":"+state);var input=label.checkboxElement;input.state=state;input.addEventListener("click",clickListener.bind(this),false);inputs.push(input);td.appendChild(label);return td;}
var tr=table.createChild("tr");tr.appendChild(createCheckbox.call(this,"active"));tr.appendChild(createCheckbox.call(this,"hover"));tr=table.createChild("tr");tr.appendChild(createCheckbox.call(this,"focus"));tr.appendChild(createCheckbox.call(this,"visited"));this._elementStatePane.appendChild(table);},_toggleAnimationsControlPane:function(event)
{event.consume();var buttonToggled=!this._animationsControlButton.classList.contains("toggled");if(buttonToggled)
this.expand();this._animationsControlButton.classList.toggle("toggled",buttonToggled);this._animationsControlPane.classList.toggle("expanded",buttonToggled);this._elementStateButton.classList.remove("toggled");this._elementStatePane.classList.remove("expanded");},_updateAnimationsPlaybackRate:function()
{function setPlaybackRate(error,playbackRate)
{this._animationsPlaybackSlider.value=WebInspector.AnimationsSidebarPane.GlobalPlaybackRates.indexOf(playbackRate);this._animationsPlaybackLabel.textContent=playbackRate+"x";}
PageAgent.animationsPlaybackRate(setPlaybackRate.bind(this));},_createAnimationsControlPane:function()
{function playbackSliderInputHandler(event)
{this._animationsPlaybackRate=WebInspector.AnimationsSidebarPane.GlobalPlaybackRates[event.target.value];PageAgent.setAnimationsPlaybackRate(this._animationsPaused?0:this._animationsPlaybackRate);this._animationsPlaybackLabel.textContent=this._animationsPlaybackRate+"x";WebInspector.userMetrics.AnimationsPlaybackRateChanged.record();}
function pauseButtonHandler()
{this._animationsPaused=!this._animationsPaused;PageAgent.setAnimationsPlaybackRate(this._animationsPaused?0:this._animationsPlaybackRate);WebInspector.userMetrics.AnimationsPlaybackRateChanged.record();this._animationsPauseButton.element.classList.toggle("pause-status-bar-item");this._animationsPauseButton.element.classList.toggle("play-status-bar-item");}
this._animationsPaused=false;this._animationsPlaybackRate=1;this._updateAnimationsPlaybackRate();this._animationsControlPane=createElementWithClass("div","styles-animations-controls-pane");var labelElement=createElement("div");labelElement.createTextChild("Animations");this._animationsControlPane.appendChild(labelElement);var container=this._animationsControlPane.createChild("div","animations-controls");var statusBar=new WebInspector.StatusBar();this._animationsPauseButton=new WebInspector.StatusBarButton("","pause-status-bar-item");statusBar.appendStatusBarItem(this._animationsPauseButton);this._animationsPauseButton.addEventListener("click",pauseButtonHandler.bind(this));container.appendChild(statusBar.element);this._animationsPlaybackSlider=container.createChild("input");this._animationsPlaybackSlider.type="range";this._animationsPlaybackSlider.min=0;this._animationsPlaybackSlider.max=WebInspector.AnimationsSidebarPane.GlobalPlaybackRates.length-1;this._animationsPlaybackSlider.value=this._animationsPlaybackSlider.max;this._animationsPlaybackSlider.addEventListener("input",playbackSliderInputHandler.bind(this));this._animationsPlaybackLabel=container.createChild("div","playback-label");this._animationsPlaybackLabel.createTextChild("1x");},filterRegex:function()
{return this._filterRegex;},_updateFilter:function()
{for(var pseudoId in this.sections){var sections=this.sections[pseudoId];for(var i=0;i<sections.length;++i){var section=sections[i];section._updateFilter();}}},_showUserAgentStylesSettingChanged:function(event)
{var showStyles=(event.data);this.element.classList.toggle("show-user-styles",showStyles);},wasShown:function()
{WebInspector.SidebarPane.prototype.wasShown.call(this);this.element.ownerDocument.body.addEventListener("keydown",this._keyDownBound,false);this.element.ownerDocument.body.addEventListener("keyup",this._keyUpBound,false);if(this._updateWhenVisible){this._rebuildUpdate();delete this._updateWhenVisible;}},willHide:function()
{this.element.ownerDocument.body.removeEventListener("keydown",this._keyDownBound,false);this.element.ownerDocument.body.removeEventListener("keyup",this._keyUpBound,false);this._spectrumHelper.hide();this._discardElementUnderMouse();WebInspector.SidebarPane.prototype.willHide.call(this);},_discardElementUnderMouse:function()
{if(this._elementUnderMouse)
this._elementUnderMouse.classList.remove("styles-panel-hovered");delete this._elementUnderMouse;},_mouseMovedOverElement:function(event)
{if(this._elementUnderMouse&&event.target!==this._elementUnderMouse)
this._discardElementUnderMouse();this._elementUnderMouse=event.target;if(WebInspector.KeyboardShortcut.eventHasCtrlOrMeta((event)))
this._elementUnderMouse.classList.add("styles-panel-hovered");},_keyDown:function(event)
{if((!WebInspector.isMac()&&event.keyCode===WebInspector.KeyboardShortcut.Keys.Ctrl.code)||(WebInspector.isMac()&&event.keyCode===WebInspector.KeyboardShortcut.Keys.Meta.code)){if(this._elementUnderMouse)
this._elementUnderMouse.classList.add("styles-panel-hovered");}},_keyUp:function(event)
{if((!WebInspector.isMac()&&event.keyCode===WebInspector.KeyboardShortcut.Keys.Ctrl.code)||(WebInspector.isMac()&&event.keyCode===WebInspector.KeyboardShortcut.Keys.Meta.code)){this._discardElementUnderMouse();}},__proto__:WebInspector.SidebarPane.prototype}
WebInspector.StylesSidebarPane._createPropertyFilterElement=function(placeholder,filterCallback)
{var input=createElement("input");input.type="text";input.placeholder=placeholder;function searchHandler()
{var regex=input.value?new RegExp(input.value.escapeForRegExp(),"i"):null;filterCallback(regex);input.parentNode.classList.toggle("styles-filter-engaged",!!input.value);}
input.addEventListener("input",searchHandler,false);function keydownHandler(event)
{var Esc="U+001B";if(event.keyIdentifier!==Esc||!input.value)
return;event.consume(true);input.value="";searchHandler();}
input.addEventListener("keydown",keydownHandler,false);return input;}
WebInspector.ComputedStyleSidebarPane=function()
{WebInspector.ElementsSidebarPane.call(this,WebInspector.UIString("Computed Style"));WebInspector.settings.showInheritedComputedStyleProperties.addChangeListener(this._showInheritedComputedStyleChanged.bind(this));this._linkifier=new WebInspector.Linkifier(new WebInspector.Linkifier.DefaultCSSFormatter());}
WebInspector.ComputedStyleSidebarPane.prototype={_showInheritedComputedStyleChanged:function()
{this._computedStyleSection.update();this._computedStyleSection._rebuildComputedTrace();},setNode:function(node)
{if(node)
this._target=node.target();WebInspector.ElementsSidebarPane.prototype.setNode.call(this,node);},doUpdate:function(finishedCallback)
{var promises=[this._stylesSidebarPane._fetchComputedCascade(),this._stylesSidebarPane._fetchMatchedCascade(),this._stylesSidebarPane._fetchAnimationProperties()];Promise.all(promises).spread(this._innerRebuildUpdate.bind(this)).then(finishedCallback);},_innerRebuildUpdate:function(computedCascade,cascades,animationProperties)
{this._linkifier.reset();this.bodyElement.removeChildren();if(!computedCascade||!cascades)
return;var computedStyleRule=computedCascade.sectionModels()[0];this._computedStyleSection=new WebInspector.ComputedStylePropertiesSection(this,computedStyleRule,cascades.matched,animationProperties);this._computedStyleSection.expand();this._computedStyleSection._rebuildComputedTrace();this.bodyElement.appendChild(this._computedStyleSection.element);},_updateFilter:function()
{this._computedStyleSection._updateFilter();},setHostingPane:function(pane)
{this._stylesSidebarPane=pane;},setFilterBoxContainer:function(element)
{element.appendChild(WebInspector.StylesSidebarPane._createPropertyFilterElement(WebInspector.UIString("Filter"),filterCallback.bind(this)));function filterCallback(regex)
{this._filterRegex=regex;this._updateFilter();}},filterRegex:function()
{return this._filterRegex;},__proto__:WebInspector.ElementsSidebarPane.prototype}
WebInspector.StylePropertiesSection=function(parentPane,styleRule)
{WebInspector.Section.call(this,"");this._parentPane=parentPane;this.styleRule=styleRule;this.editable=styleRule.editable();var rule=styleRule.rule();var extraClasses=(rule&&(rule.isInjected||rule.isUserAgent)?" user-rule":"");this.element.className="styles-section matched-styles monospace"+extraClasses;this.propertiesElement.classList.remove("properties-tree");var selectorContainer=createElement("div");this._selectorElement=createElement("span");this._selectorElement.textContent=styleRule.selectorText();selectorContainer.appendChild(this._selectorElement);var openBrace=createElement("span");openBrace.textContent=" {";selectorContainer.appendChild(openBrace);selectorContainer.addEventListener("mousedown",this._handleEmptySpaceMouseDown.bind(this),false);selectorContainer.addEventListener("click",this._handleSelectorContainerClick.bind(this),false);var closeBrace=createElement("div");closeBrace.textContent="}";this.element.appendChild(closeBrace);if(this.editable&&rule){var newRuleButton=closeBrace.createChild("div","sidebar-pane-button-new-rule");newRuleButton.title=WebInspector.UIString("Insert Style Rule");newRuleButton.addEventListener("click",this._onNewRuleClick.bind(this),false);}
this._selectorElement.addEventListener("click",this._handleSelectorClick.bind(this),false);this.element.addEventListener("mousedown",this._handleEmptySpaceMouseDown.bind(this),false);this.element.addEventListener("click",this._handleEmptySpaceClick.bind(this),false);if(rule){if(rule.isUserAgent||rule.isInjected)
this.editable=false;else{if(rule.styleSheetId)
this.navigable=!!rule.resourceURL();}
this.titleElement.classList.add("styles-selector");}
this._selectorRefElement=createElement("div");this._selectorRefElement.className="subtitle";this._mediaListElement=this.titleElement.createChild("div","media-list media-matches");this._updateMediaList();this._updateRuleOrigin();selectorContainer.insertBefore(this._selectorRefElement,selectorContainer.firstChild);this.titleElement.appendChild(selectorContainer);this._selectorContainer=selectorContainer;if(this.navigable)
this.element.classList.add("navigable");if(!this.editable)
this.element.classList.add("read-only");}
WebInspector.StylePropertiesSection.prototype={firstSibling:function()
{var parent=this.element.parentElement;if(!parent)
return null;var childElement=parent.firstChild;while(childElement){if(childElement._section)
return childElement._section;childElement=childElement.nextSibling;}
return null;},lastSibling:function()
{var parent=this.element.parentElement;if(!parent)
return null;var childElement=parent.lastChild;while(childElement){if(childElement._section)
return childElement._section;childElement=childElement.previousSibling;}
return null;},nextSibling:function()
{var curElement=this.element;do{curElement=curElement.nextSibling;}while(curElement&&!curElement._section);return curElement?curElement._section:null;},previousSibling:function()
{var curElement=this.element;do{curElement=curElement.previousSibling;}while(curElement&&!curElement._section);return curElement?curElement._section:null;},inherited:function()
{return this.styleRule.inherited();},rule:function()
{return this.styleRule.rule();},_onNewRuleClick:function(event)
{event.consume();var rule=this.rule();var range=WebInspector.TextRange.createFromLocation(rule.style.range.endLine,rule.style.range.endColumn+1);this._parentPane._addBlankSection(this,(rule.styleSheetId),range);},_styleSheetRuleEdited:function(editedRule,oldRange,newRange)
{var rule=this.rule();if(!rule||!rule.styleSheetId)
return;if(rule!==editedRule)
rule.sourceStyleSheetEdited((editedRule.styleSheetId),oldRange,newRange);this._updateMediaList();this._updateRuleOrigin();},_styleSheetMediaEdited:function(oldMedia,newMedia)
{var rule=this.rule();if(!rule||!rule.styleSheetId)
return;rule.mediaEdited(oldMedia,newMedia);this._updateMediaList();},_createMediaList:function(mediaRules)
{if(!mediaRules)
return;for(var i=mediaRules.length-1;i>=0;--i){var media=mediaRules[i];var mediaDataElement=this._mediaListElement.createChild("div","media");if(media.sourceURL){var refElement=mediaDataElement.createChild("div","subtitle");var anchor=this._parentPane._linkifier.linkifyMedia(media);anchor.style.float="right";refElement.appendChild(anchor);}
var mediaContainerElement=mediaDataElement.createChild("span");var mediaTextElement=mediaContainerElement.createChild("span","media-text");mediaTextElement.title=media.text;switch(media.source){case WebInspector.CSSMedia.Source.LINKED_SHEET:case WebInspector.CSSMedia.Source.INLINE_SHEET:mediaTextElement.textContent="media=\""+media.text+"\"";break;case WebInspector.CSSMedia.Source.MEDIA_RULE:var decoration=mediaContainerElement.createChild("span");mediaContainerElement.insertBefore(decoration,mediaTextElement);decoration.textContent="@media ";decoration.title=media.text;mediaTextElement.textContent=media.text;if(media.parentStyleSheetId){mediaDataElement.classList.add("editable-media");mediaTextElement.addEventListener("click",this._handleMediaRuleClick.bind(this,media,mediaTextElement),false);}
break;case WebInspector.CSSMedia.Source.IMPORT_RULE:mediaTextElement.textContent="@import "+media.text;break;}}},_updateMediaList:function()
{this._mediaListElement.removeChildren();this._createMediaList(this.styleRule.media());},collapse:function()
{},handleClick:function()
{},isPropertyInherited:function(propertyName)
{if(this.inherited()){return!WebInspector.CSSMetadata.isPropertyInherited(propertyName);}
return false;},nextEditableSibling:function()
{var curSection=this;do{curSection=curSection.nextSibling();}while(curSection&&!curSection.editable);if(!curSection){curSection=this.firstSibling();while(curSection&&!curSection.editable)
curSection=curSection.nextSibling();}
return(curSection&&curSection.editable)?curSection:null;},previousEditableSibling:function()
{var curSection=this;do{curSection=curSection.previousSibling();}while(curSection&&!curSection.editable);if(!curSection){curSection=this.lastSibling();while(curSection&&!curSection.editable)
curSection=curSection.previousSibling();}
return(curSection&&curSection.editable)?curSection:null;},update:function(full)
{if(this.styleRule.selectorText())
this._selectorElement.textContent=this.styleRule.selectorText();this._markSelectorMatches();if(full){this.propertiesTreeOutline.removeChildren();this.repopulate();}else{var child=this.propertiesTreeOutline.children[0];while(child){child.overloaded=this.styleRule.isPropertyOverloaded(child.name,child.isShorthand);child=child.traverseNextTreeElement(false,null,true);}}
this.afterUpdate();},afterUpdate:function()
{if(this._afterUpdate){this._afterUpdate(this);delete this._afterUpdate;this._afterUpdateFinishedForTest();}},_afterUpdateFinishedForTest:function()
{},onpopulate:function()
{var style=this.styleRule.style();var allProperties=style.allProperties;var styleHasEditableSource=this.editable&&!!style.range;if(styleHasEditableSource){for(var i=0;i<allProperties.length;++i){var property=allProperties[i];if(property.styleBased)
continue;var isShorthand=!!WebInspector.CSSMetadata.cssPropertiesMetainfo.longhands(property.name);var inherited=this.isPropertyInherited(property.name);var overloaded=property.inactive||this.styleRule.isPropertyOverloaded(property.name);var item=new WebInspector.StylePropertyTreeElement(this._parentPane,this.styleRule,property,isShorthand,inherited,overloaded);this.propertiesTreeOutline.appendChild(item);}
return;}
var generatedShorthands={};for(var i=0;i<allProperties.length;++i){var property=allProperties[i];var isShorthand=!!WebInspector.CSSMetadata.cssPropertiesMetainfo.longhands(property.name);var shorthands=isShorthand?null:WebInspector.CSSMetadata.cssPropertiesMetainfo.shorthands(property.name);var shorthandPropertyAvailable=false;for(var j=0;shorthands&&!shorthandPropertyAvailable&&j<shorthands.length;++j){var shorthand=shorthands[j];if(shorthand in generatedShorthands){shorthandPropertyAvailable=true;continue;}
if(style.getLiveProperty(shorthand)){shorthandPropertyAvailable=true;continue;}
if(!style.shorthandValue(shorthand)){shorthandPropertyAvailable=false;continue;}
var shorthandProperty=new WebInspector.CSSProperty(style,style.allProperties.length,shorthand,style.shorthandValue(shorthand),false,false,true,true);var overloaded=property.inactive||this.styleRule.isPropertyOverloaded(property.name,true);var item=new WebInspector.StylePropertyTreeElement(this._parentPane,this.styleRule,shorthandProperty,true,false,overloaded);this.propertiesTreeOutline.appendChild(item);generatedShorthands[shorthand]=shorthandProperty;shorthandPropertyAvailable=true;}
if(shorthandPropertyAvailable)
continue;var inherited=this.isPropertyInherited(property.name);var overloaded=property.inactive||this.styleRule.isPropertyOverloaded(property.name,isShorthand);var item=new WebInspector.StylePropertyTreeElement(this._parentPane,this.styleRule,property,isShorthand,inherited,overloaded);this.propertiesTreeOutline.appendChild(item);}},_updateFilter:function()
{if(this.styleRule.isAttribute())
return;var regex=this._parentPane.filterRegex();var hideRule=regex&&!regex.test(this.element.textContent);this.element.classList.toggle("hidden",hideRule);if(hideRule)
return;var children=this.propertiesTreeOutline.children;for(var i=0;i<children.length;++i)
children[i]._updateFilter();if(this.styleRule.rule())
this._markSelectorHighlights();},_markSelectorMatches:function()
{var rule=this.styleRule.rule();if(!rule)
return;this._mediaListElement.classList.toggle("media-matches",this.styleRule.mediaMatches());if(!this.styleRule.hasMatchingSelectors()){this._selectorElement.className="selector";return;}
var selectors=rule.selectors;var fragment=createDocumentFragment();var currentMatch=0;var matchingSelectors=rule.matchingSelectors;for(var i=0;i<selectors.length;++i){if(i)
fragment.createTextChild(", ");var isSelectorMatching=matchingSelectors[currentMatch]===i;if(isSelectorMatching)
++currentMatch;var matchingSelectorClass=isSelectorMatching?" selector-matches":"";var selectorElement=createElement("span");selectorElement.className="simple-selector"+matchingSelectorClass;if(rule.styleSheetId)
selectorElement._selectorIndex=i;selectorElement.textContent=selectors[i].value;fragment.appendChild(selectorElement);}
this._selectorElement.removeChildren();this._selectorElement.appendChild(fragment);this._markSelectorHighlights();},_markSelectorHighlights:function()
{var selectors=this._selectorElement.getElementsByClassName("simple-selector");var regex=this._parentPane.filterRegex();for(var i=0;i<selectors.length;++i){var selectorMatchesFilter=regex&&regex.test(selectors[i].textContent);selectors[i].classList.toggle("filter-match",selectorMatchesFilter);}},_checkWillCancelEditing:function()
{var willCauseCancelEditing=this._willCauseCancelEditing;delete this._willCauseCancelEditing;return willCauseCancelEditing;},_handleSelectorContainerClick:function(event)
{if(this._checkWillCancelEditing()||!this.editable)
return;if(event.target===this._selectorContainer){this.addNewBlankProperty(0).startEditing();event.consume(true);}},addNewBlankProperty:function(index)
{var property=this.styleRule.style().newBlankProperty(index);var item=new WebInspector.StylePropertyTreeElement(this._parentPane,this.styleRule,property,false,false,false);index=property.index;this.propertiesTreeOutline.insertChild(item,index);item.listItemElement.textContent="";item._newProperty=true;item.updateTitle();return item;},_handleEmptySpaceMouseDown:function()
{this._willCauseCancelEditing=this._parentPane._isEditingStyle;},_handleEmptySpaceClick:function(event)
{if(!this.editable)
return;if(!event.target.window().getSelection().isCollapsed)
return;if(this._checkWillCancelEditing())
return;if(event.target.classList.contains("header")||this.element.classList.contains("read-only")||event.target.enclosingNodeOrSelfWithClass("media")){event.consume();return;}
this.expand();this.addNewBlankProperty().startEditing();event.consume(true);},_handleMediaRuleClick:function(media,element,event)
{if(WebInspector.isBeingEdited(element))
return;var config=new WebInspector.InplaceEditor.Config(this._editingMediaCommitted.bind(this,media),this._editingMediaCancelled.bind(this,element),undefined,this._editingMediaBlurHandler.bind(this));WebInspector.InplaceEditor.startEditing(element,config);element.window().getSelection().setBaseAndExtent(element,0,element,1);this._parentPane._isEditingStyle=true;var parentMediaElement=element.enclosingNodeOrSelfWithClass("media");parentMediaElement.classList.add("editing-media");event.consume(true);},_editingMediaFinished:function(element)
{delete this._parentPane._isEditingStyle;var parentMediaElement=element.enclosingNodeOrSelfWithClass("media");parentMediaElement.classList.remove("editing-media");},_editingMediaCancelled:function(element)
{this._editingMediaFinished(element);this._markSelectorMatches();element.window().getSelection().collapse(element,0);},_editingMediaBlurHandler:function(editor,blurEvent)
{return true;},_editingMediaCommitted:function(media,element,newContent,oldContent,context,moveDirection)
{delete this._parentPane._isEditingStyle;this._editingMediaFinished(element);if(newContent)
newContent=newContent.trim();function successCallback(newMedia)
{this._parentPane._styleSheetMediaEdited(media,newMedia);this._parentPane._refreshUpdate(this);finishOperation.call(this);}
function finishOperation()
{delete this._parentPane._userOperation;this._editingMediaTextCommittedForTest();}
this._parentPane._userOperation=true;this._parentPane._target.cssModel.setMediaText(media,newContent,successCallback.bind(this),finishOperation.bind(this));},_editingMediaTextCommittedForTest:function(){},_handleSelectorClick:function(event)
{if(WebInspector.KeyboardShortcut.eventHasCtrlOrMeta((event))&&this.navigable&&event.target.classList.contains("simple-selector")){var index=event.target._selectorIndex;var target=this._parentPane._target;var rule=this.rule();var rawLocation=new WebInspector.CSSLocation(target,(rule.styleSheetId),rule.sourceURL,rule.lineNumberInSource(index),rule.columnNumberInSource(index));var uiLocation=WebInspector.cssWorkspaceBinding.rawLocationToUILocation(rawLocation);if(uiLocation)
WebInspector.Revealer.reveal(uiLocation);event.consume(true);return;}
this._startEditingOnMouseEvent();event.consume(true);},_startEditingOnMouseEvent:function()
{if(!this.editable)
return;if(!this.rule()&&this.propertiesTreeOutline.children.length===0){this.expand();this.addNewBlankProperty().startEditing();return;}
if(!this.rule())
return;this.startEditingSelector();},startEditingSelector:function()
{var element=this._selectorElement;if(WebInspector.isBeingEdited(element))
return;element.scrollIntoViewIfNeeded(false);element.textContent=element.textContent;var config=new WebInspector.InplaceEditor.Config(this.editingSelectorCommitted.bind(this),this.editingSelectorCancelled.bind(this),undefined,this._editingSelectorBlurHandler.bind(this));WebInspector.InplaceEditor.startEditing(this._selectorElement,config);element.window().getSelection().setBaseAndExtent(element,0,element,1);this._parentPane._isEditingStyle=true;this._parentPane._startEditingSelector(this);},setSelectorText:function(text)
{this._selectorElement.textContent=text;this._selectorElement.window().getSelection().setBaseAndExtent(this._selectorElement,0,this._selectorElement,1);},_editingSelectorBlurHandler:function(editor,blurEvent)
{if(!blurEvent.relatedTarget)
return true;var elementTreeOutline=blurEvent.relatedTarget.enclosingNodeOrSelfWithClass("elements-tree-outline");if(!elementTreeOutline)
return true;editor.focus();return false;},_moveEditorFromSelector:function(moveDirection)
{this._markSelectorMatches();if(!moveDirection)
return;if(moveDirection==="forward"){this.expand();var firstChild=this.propertiesTreeOutline.children[0];while(firstChild&&firstChild.inherited)
firstChild=firstChild.nextSibling;if(!firstChild)
this.addNewBlankProperty().startEditing();else
firstChild.startEditing(firstChild.nameElement);}else{var previousSection=this.previousEditableSibling();if(!previousSection)
return;previousSection.expand();previousSection.addNewBlankProperty().startEditing();}},editingSelectorCommitted:function(element,newContent,oldContent,context,moveDirection)
{this._editingSelectorEnded();if(newContent)
newContent=newContent.trim();if(newContent===oldContent){this._selectorElement.textContent=newContent;this._moveEditorFromSelector(moveDirection);return;}
function successCallback(newRule)
{var doesAffectSelectedNode=newRule.matchingSelectors.length>0;this.element.classList.toggle("no-affect",!doesAffectSelectedNode);var oldSelectorRange=this.rule().selectorRange;this.styleRule.updateRule(newRule);this._parentPane._refreshUpdate(this);this._parentPane._styleSheetRuleEdited(newRule,oldSelectorRange,newRule.selectorRange);finishOperationAndMoveEditor.call(this,moveDirection);}
function finishOperationAndMoveEditor(direction)
{delete this._parentPane._userOperation;this._moveEditorFromSelector(direction);this._editingSelectorCommittedForTest();}
this._parentPane._userOperation=true;var selectedNode=this._parentPane._node;this._parentPane._target.cssModel.setRuleSelector(this.rule(),selectedNode?selectedNode.id:0,newContent,successCallback.bind(this),finishOperationAndMoveEditor.bind(this,moveDirection));},_editingSelectorCommittedForTest:function(){},_updateRuleOrigin:function()
{this._selectorRefElement.removeChildren();this._selectorRefElement.appendChild(WebInspector.StylePropertiesSection._createRuleOriginNode(this._parentPane._target,this._parentPane._linkifier,this.rule()));},_editingSelectorEnded:function()
{delete this._parentPane._isEditingStyle;this._parentPane._finishEditingSelector();},editingSelectorCancelled:function()
{this._editingSelectorEnded();this._markSelectorMatches();},__proto__:WebInspector.Section.prototype}
WebInspector.StylePropertiesSection._createRuleOriginNode=function(target,linkifier,rule)
{if(!rule)
return createTextNode("");var firstMatchingIndex=rule.matchingSelectors&&rule.matchingSelectors.length?rule.matchingSelectors[0]:0;var ruleLocation=rule.selectors[firstMatchingIndex].range;var header=rule.styleSheetId?target.cssModel.styleSheetHeaderForId(rule.styleSheetId):null;if(ruleLocation&&rule.styleSheetId&&header&&header.resourceURL())
return WebInspector.StylePropertiesSection._linkifyRuleLocation(target,linkifier,rule.styleSheetId,ruleLocation);if(rule.isUserAgent)
return createTextNode(WebInspector.UIString("user agent stylesheet"));if(rule.isInjected)
return createTextNode(WebInspector.UIString("injected stylesheet"));if(rule.isViaInspector)
return createTextNode(WebInspector.UIString("via inspector"));if(header&&header.ownerNode){var link=WebInspector.DOMPresentationUtils.linkifyDeferredNodeReference(header.ownerNode);link.textContent="<style>…</style>";return link;}
return createTextNode("");}
WebInspector.StylePropertiesSection._linkifyRuleLocation=function(target,linkifier,styleSheetId,ruleLocation)
{var styleSheetHeader=target.cssModel.styleSheetHeaderForId(styleSheetId);var sourceURL=styleSheetHeader.resourceURL();var lineNumber=styleSheetHeader.lineNumberInSource(ruleLocation.startLine);var columnNumber=styleSheetHeader.columnNumberInSource(ruleLocation.startLine,ruleLocation.startColumn);var matchingSelectorLocation=new WebInspector.CSSLocation(target,styleSheetId,sourceURL,lineNumber,columnNumber);return linkifier.linkifyCSSLocation(matchingSelectorLocation);}
WebInspector.ComputedStylePropertiesSection=function(stylesPane,styleRule,matchedRuleCascade,animationProperties)
{WebInspector.Section.call(this,"");this.element.className="styles-section monospace read-only computed-style";this.headerElement.appendChild(WebInspector.ComputedStylePropertiesSection._showInheritedCheckbox());this._stylesPane=stylesPane;this.styleRule=styleRule;this._matchedRuleCascade=matchedRuleCascade;this._animationProperties=animationProperties;this._alwaysShowComputedProperties={"display":true,"height":true,"width":true};this._propertyTreeElements={};this._expandedPropertyNames={};}
WebInspector.ComputedStylePropertiesSection._showInheritedCheckbox=function()
{if(!WebInspector.ComputedStylePropertiesSection._showInheritedCheckboxElement){WebInspector.ComputedStylePropertiesSection._showInheritedCheckboxElement=WebInspector.SettingsUI.createSettingCheckbox(WebInspector.UIString("Show inherited properties"),WebInspector.settings.showInheritedComputedStyleProperties,true);WebInspector.ComputedStylePropertiesSection._showInheritedCheckboxElement.classList.add("checkbox-with-label");}
return WebInspector.ComputedStylePropertiesSection._showInheritedCheckboxElement;}
WebInspector.ComputedStylePropertiesSection.prototype={collapse:function()
{},_isPropertyInherited:function(propertyName)
{var canonicalName=WebInspector.CSSMetadata.canonicalPropertyName(propertyName);return!(this._matchedRuleCascade.allUsedProperties().has(canonicalName))&&!(canonicalName in this._alwaysShowComputedProperties)&&!this._animationProperties.has(canonicalName);},update:function()
{this._expandedPropertyNames={};for(var name in this._propertyTreeElements){if(this._propertyTreeElements[name].expanded)
this._expandedPropertyNames[name]=true;}
this._propertyTreeElements={};this.propertiesTreeOutline.removeChildren();this.repopulate();},_updateFilter:function()
{var children=this.propertiesTreeOutline.children;for(var i=0;i<children.length;++i)
children[i]._updateFilter();},onpopulate:function()
{var style=this.styleRule.style();if(!style)
return;var uniqueProperties=[];var allProperties=style.allProperties;for(var i=0;i<allProperties.length;++i)
uniqueProperties.push(allProperties[i]);uniqueProperties.sort(propertySorter);this._propertyTreeElements={};var showInherited=WebInspector.settings.showInheritedComputedStyleProperties.get();for(var i=0;i<uniqueProperties.length;++i){var property=uniqueProperties[i];var inherited=this._isPropertyInherited(property.name);if(!showInherited&&inherited)
continue;var item=new WebInspector.ComputedStylePropertyTreeElement(this._stylesPane,this.styleRule,property,inherited);this.propertiesTreeOutline.appendChild(item);this._propertyTreeElements[property.name]=item;}
function propertySorter(a,b)
{return a.name.compareTo(b.name);}},_rebuildComputedTrace:function()
{for(var property of this._animationProperties.keys()){var treeElement=this._propertyTreeElements[property.toLowerCase()];if(treeElement){var fragment=createDocumentFragment();var name=fragment.createChild("span");name.textContent=WebInspector.UIString("Animation")+" "+this._animationProperties.get(property);treeElement.appendChild(new TreeElement(fragment,null,false));}}
for(var model of this._matchedRuleCascade.sectionModels()){var properties=model.style().allProperties;for(var j=0;j<properties.length;++j){var property=properties[j];if(property.disabled)
continue;if(model.inherited()&&!WebInspector.CSSMetadata.isPropertyInherited(property.name))
continue;var treeElement=this._propertyTreeElements[property.name.toLowerCase()];if(treeElement){var fragment=createDocumentFragment();var selector=fragment.createChild("span");selector.style.color="gray";selector.textContent=model.selectorText();fragment.createTextChild(" - "+property.value+" ");var subtitle=fragment.createChild("span");subtitle.style.float="right";subtitle.appendChild(WebInspector.StylePropertiesSection._createRuleOriginNode(this._stylesPane._target,this._stylesPane._linkifier,model.rule()));var childElement=new TreeElement(fragment,null,false);treeElement.appendChild(childElement);if(property.inactive||model.isPropertyOverloaded(property.name))
childElement.listItemElement.classList.add("overloaded");if(!property.parsedOk){childElement.listItemElement.classList.add("not-parsed-ok");childElement.listItemElement.insertBefore(WebInspector.StylesSidebarPane.createExclamationMark(property),childElement.listItemElement.firstChild);if(WebInspector.StylesSidebarPane._ignoreErrorsForProperty(property))
childElement.listItemElement.classList.add("has-ignorable-error");}}}}
for(var name in this._expandedPropertyNames){if(name in this._propertyTreeElements)
this._propertyTreeElements[name].expand();}},__proto__:WebInspector.Section.prototype}
WebInspector.BlankStylePropertiesSection=function(stylesPane,defaultSelectorText,styleSheetId,ruleLocation,insertAfterStyleRule)
{var styleSheetHeader=WebInspector.cssModel.styleSheetHeaderForId(styleSheetId);var dummyCascade=new WebInspector.SectionCascade();var blankSectionModel=dummyCascade.appendModelFromStyle(WebInspector.CSSStyleDeclaration.createDummyStyle(),defaultSelectorText);blankSectionModel.setEditable(true);WebInspector.StylePropertiesSection.call(this,stylesPane,blankSectionModel);this._ruleLocation=ruleLocation;this._styleSheetId=styleSheetId;this._selectorRefElement.removeChildren();this._selectorRefElement.appendChild(WebInspector.StylePropertiesSection._linkifyRuleLocation(this._parentPane._target,this._parentPane._linkifier,styleSheetId,this._actualRuleLocation()));if(insertAfterStyleRule)
this._createMediaList(insertAfterStyleRule.media());this._insertAfterStyleRule=insertAfterStyleRule;this.element.classList.add("blank-section");}
WebInspector.BlankStylePropertiesSection.prototype={_actualRuleLocation:function()
{var prefix=this._rulePrefix();var lines=prefix.split("\n");var editRange=new WebInspector.TextRange(0,0,lines.length-1,lines.peekLast().length);return this._ruleLocation.rebaseAfterTextEdit(WebInspector.TextRange.createFromLocation(0,0),editRange);},_rulePrefix:function()
{return this._ruleLocation.startLine===0&&this._ruleLocation.startColumn===0?"":"\n\n";},get isBlank()
{return!this._normal;},expand:function()
{if(!this.isBlank)
WebInspector.StylePropertiesSection.prototype.expand.call(this);},editingSelectorCommitted:function(element,newContent,oldContent,context,moveDirection)
{if(!this.isBlank){WebInspector.StylePropertiesSection.prototype.editingSelectorCommitted.call(this,element,newContent,oldContent,context,moveDirection);return;}
function successCallback(newRule)
{var doesSelectorAffectSelectedNode=newRule.matchingSelectors.length>0;this._makeNormal(newRule);if(!doesSelectorAffectSelectedNode)
this.element.classList.add("no-affect");var ruleTextLines=ruleText.split("\n");var startLine=this._ruleLocation.startLine;var startColumn=this._ruleLocation.startColumn;var newRange=new WebInspector.TextRange(startLine,startColumn,startLine+ruleTextLines.length-1,startColumn+ruleTextLines[ruleTextLines.length-1].length);this._parentPane._styleSheetRuleEdited(newRule,this._ruleLocation,newRange);this._updateRuleOrigin();this.expand();if(this.element.parentElement)
this._moveEditorFromSelector(moveDirection);delete this._parentPane._userOperation;this._editingSelectorEnded();this._markSelectorMatches();this._editingSelectorCommittedForTest();}
function failureCallback()
{this.editingSelectorCancelled();this._editingSelectorCommittedForTest();}
if(newContent)
newContent=newContent.trim();this._parentPane._userOperation=true;var cssModel=this._parentPane._target.cssModel;var ruleText=this._rulePrefix()+newContent+" {}";cssModel.addRule(this._styleSheetId,this._parentPane._node,ruleText,this._ruleLocation,successCallback.bind(this),failureCallback.bind(this));},editingSelectorCancelled:function()
{delete this._parentPane._userOperation;if(!this.isBlank){WebInspector.StylePropertiesSection.prototype.editingSelectorCancelled.call(this);return;}
this._editingSelectorEnded();this._parentPane.removeSection(this);},_makeNormal:function(newRule)
{this.element.classList.remove("blank-section");var model=this._insertAfterStyleRule.cascade().insertModelFromRule(newRule,this._insertAfterStyleRule);this.styleRule=model;this._normal=true;},__proto__:WebInspector.StylePropertiesSection.prototype}
WebInspector.StylePropertyTreeElementBase=function(styleRule,property,inherited,overloaded,hasChildren)
{this._styleRule=styleRule;this.property=property;this._inherited=inherited;this._overloaded=overloaded;TreeElement.call(this,"",null,hasChildren);this.selectable=false;}
WebInspector.StylePropertyTreeElementBase.prototype={style:function()
{return this._styleRule.style();},node:function()
{return null;},editablePane:function()
{return null;},parentPane:function()
{throw"Not implemented";},get inherited()
{return this._inherited;},hasIgnorableError:function()
{return!this.parsedOk&&WebInspector.StylesSidebarPane._ignoreErrorsForProperty(this.property);},set inherited(x)
{if(x===this._inherited)
return;this._inherited=x;this.updateState();},get overloaded()
{return this._overloaded;},set overloaded(x)
{if(x===this._overloaded)
return;this._overloaded=x;this.updateState();},get disabled()
{return this.property.disabled;},get name()
{if(!this.disabled||!this.property.text)
return this.property.name;var text=this.property.text;var index=text.indexOf(":");if(index<1)
return this.property.name;text=text.substring(0,index).trim();if(text.startsWith("/*"))
text=text.substring(2).trim();return text;},get value()
{if(!this.disabled||!this.property.text)
return this.property.value;var match=this.property.text.match(/(.*);\s*/);if(!match||!match[1])
return this.property.value;var text=match[1];var index=text.indexOf(":");if(index<1)
return this.property.value;return text.substring(index+1).trim();},get parsedOk()
{return this.property.parsedOk;},onattach:function()
{this.updateTitle();},updateTitle:function()
{var value=this.value;this.updateState();var nameElement=createElement("span");nameElement.className="webkit-css-property";nameElement.textContent=this.name;nameElement.title=this.property.propertyText;this.nameElement=nameElement;this._expandElement=createElement("span");this._expandElement.className="expand-element";var valueElement=createElement("span");valueElement.className="value";this.valueElement=valueElement;function processValue(regex,processor,nextProcessor,valueText)
{var container=createDocumentFragment();var items=valueText.replace(regex,"\0$1\0").split("\0");for(var i=0;i<items.length;++i){if((i%2)===0){if(nextProcessor)
container.appendChild(nextProcessor(items[i]));else
container.createTextChild(items[i]);}else{var processedNode=processor(items[i]);if(processedNode)
container.appendChild(processedNode);}}
return container;}
function urlRegex(value)
{if(/url\(\s*'.*\s*'\s*\)/.test(value))
return/url\(\s*('.+')\s*\)/g;if(/url\(\s*".*\s*"\s*\)/.test(value))
return/url\(\s*(".+")\s*\)/g;return/url\(\s*([^)]+)\s*\)/g;}
function linkifyURL(url)
{var hrefUrl=url;var match=hrefUrl.match(/['"]?([^'"]+)/);if(match)
hrefUrl=match[1];var container=createDocumentFragment();container.createTextChild("url(");if(this._styleRule.rule()&&this._styleRule.rule().resourceURL())
hrefUrl=WebInspector.ParsedURL.completeURL(this._styleRule.rule().resourceURL(),hrefUrl);else if(this.node())
hrefUrl=this.node().resolveURL(hrefUrl);var hasResource=hrefUrl&&!!WebInspector.resourceForURL(hrefUrl);container.appendChild(WebInspector.linkifyURLAsNode(hrefUrl||url,url,undefined,!hasResource));container.createTextChild(")");return container;}
if(value){var colorProcessor=processValue.bind(null,WebInspector.StylesSidebarPane._colorRegex,this._processColor.bind(this,nameElement,valueElement),null);valueElement.appendChild(processValue(urlRegex(value),linkifyURL.bind(this),WebInspector.CSSMetadata.isColorAwareProperty(this.name)&&this.parsedOk?colorProcessor:null,value));}
this.listItemElement.removeChildren();nameElement.normalize();valueElement.normalize();if(!this.treeOutline)
return;this.listItemElement.createChild("span","styles-clipboard-only").createTextChild(this.disabled?"  /* ":"  ");this.listItemElement.appendChild(nameElement);this.listItemElement.createTextChild(": ");this.listItemElement.appendChild(this._expandElement);this.listItemElement.appendChild(valueElement);this.listItemElement.createTextChild(";");if(this.disabled)
this.listItemElement.createChild("span","styles-clipboard-only").createTextChild(" */");if(!this.parsedOk){this.hasChildren=false;this.listItemElement.classList.add("not-parsed-ok");this.listItemElement.insertBefore(WebInspector.StylesSidebarPane.createExclamationMark(this.property),this.listItemElement.firstChild);}
if(this.property.inactive)
this.listItemElement.classList.add("inactive");this._updateFilter();},_updateFilter:function()
{var regEx=this.parentPane().filterRegex();this.listItemElement.classList.toggle("filter-match",!!regEx&&(regEx.test(this.property.name)||regEx.test(this.property.value)));},_processColor:function(nameElement,valueElement,text)
{var color=WebInspector.Color.parse(text);if(!color)
return createTextNode(text);var format=WebInspector.StylesSidebarPane._colorFormat(color);var spectrumHelper=this.editablePane()&&this.editablePane()._spectrumHelper;var spectrum=spectrumHelper?spectrumHelper.spectrum():null;var isEditable=!!(this._styleRule&&this._styleRule.editable());var colorSwatch=new WebInspector.ColorSwatch(!isEditable);colorSwatch.setColorString(text);colorSwatch.element.addEventListener("click",swatchClick.bind(this),false);var scrollerElement;var boundSpectrumChanged=spectrumChanged.bind(this);var boundSpectrumHidden=spectrumHidden.bind(this);function spectrumChanged(event)
{var colorString=(event.data);spectrum.displayText=colorString;colorValueElement.textContent=colorString;colorSwatch.setColorString(colorString);this.applyStyleText(nameElement.textContent+": "+valueElement.textContent,false,false,false);}
function spectrumHidden(event)
{if(scrollerElement)
scrollerElement.removeEventListener("scroll",repositionSpectrum,false);var commitEdit=event.data;var propertyText=!commitEdit&&this.originalPropertyText?this.originalPropertyText:(nameElement.textContent+": "+valueElement.textContent);this.applyStyleText(propertyText,true,true,false);spectrum.removeEventListener(WebInspector.Spectrum.Events.ColorChanged,boundSpectrumChanged);spectrumHelper.removeEventListener(WebInspector.SpectrumPopupHelper.Events.Hidden,boundSpectrumHidden);delete this.editablePane()._isEditingStyle;delete this.originalPropertyText;}
function repositionSpectrum()
{spectrumHelper.reposition(colorSwatch.element);}
function swatchClick(e)
{e.consume(true);if(!spectrumHelper||e.shiftKey){changeColorDisplay();return;}
if(!isEditable)
return;var visible=spectrumHelper.toggle(colorSwatch.element,color,format);if(visible){spectrum.displayText=color.asString(format);this.originalPropertyText=this.property.propertyText;this.editablePane()._isEditingStyle=true;spectrum.addEventListener(WebInspector.Spectrum.Events.ColorChanged,boundSpectrumChanged);spectrumHelper.addEventListener(WebInspector.SpectrumPopupHelper.Events.Hidden,boundSpectrumHidden);scrollerElement=colorSwatch.element.enclosingNodeOrSelfWithClass("style-panes-wrapper");if(scrollerElement)
scrollerElement.addEventListener("scroll",repositionSpectrum,false);else
console.error("Unable to handle color picker scrolling");}}
var colorValueElement=createElement("span");if(format===WebInspector.Color.Format.Original)
colorValueElement.textContent=text;else
colorValueElement.textContent=color.asString(format);function nextFormat(curFormat)
{var cf=WebInspector.Color.Format;switch(curFormat){case cf.Original:return!color.hasAlpha()?cf.RGB:cf.RGBA;case cf.RGB:case cf.RGBA:return!color.hasAlpha()?cf.HSL:cf.HSLA;case cf.HSL:case cf.HSLA:if(color.nickname())
return cf.Nickname;if(!color.hasAlpha())
return color.canBeShortHex()?cf.ShortHEX:cf.HEX;else
return cf.Original;case cf.ShortHEX:return cf.HEX;case cf.HEX:return cf.Original;case cf.Nickname:if(!color.hasAlpha())
return color.canBeShortHex()?cf.ShortHEX:cf.HEX;else
return cf.Original;default:return cf.RGBA;}}
function changeColorDisplay()
{do{format=nextFormat(format);var currentValue=color.asString(format);}while(currentValue===colorValueElement.textContent);colorValueElement.textContent=currentValue;}
var container=createElement("nobr");container.appendChild(colorSwatch.element);container.appendChild(colorValueElement);return container;},updateState:function()
{if(!this.listItemElement)
return;if(this.style().isPropertyImplicit(this.name))
this.listItemElement.classList.add("implicit");else
this.listItemElement.classList.remove("implicit");if(this.hasIgnorableError())
this.listItemElement.classList.add("has-ignorable-error");else
this.listItemElement.classList.remove("has-ignorable-error");if(this.inherited)
this.listItemElement.classList.add("inherited");else
this.listItemElement.classList.remove("inherited");if(this.overloaded)
this.listItemElement.classList.add("overloaded");else
this.listItemElement.classList.remove("overloaded");if(this.disabled)
this.listItemElement.classList.add("disabled");else
this.listItemElement.classList.remove("disabled");},__proto__:TreeElement.prototype}
WebInspector.ComputedStylePropertyTreeElement=function(stylesPane,styleRule,property,inherited)
{WebInspector.StylePropertyTreeElementBase.call(this,styleRule,property,inherited,false,false);this._stylesPane=stylesPane;}
WebInspector.ComputedStylePropertyTreeElement.prototype={editablePane:function()
{return null;},parentPane:function()
{return this._stylesPane;},_updateFilter:function()
{var regEx=this.parentPane().filterRegex();var matched=!!regEx&&(!regEx.test(this.property.name)&&!regEx.test(this.property.value));this.listItemElement.classList.toggle("hidden",matched);if(this.childrenListElement)
this.childrenListElement.classList.toggle("hidden",matched);},__proto__:WebInspector.StylePropertyTreeElementBase.prototype}
WebInspector.StylePropertyTreeElement=function(stylesPane,styleRule,property,isShorthand,inherited,overloaded)
{WebInspector.StylePropertyTreeElementBase.call(this,styleRule,property,inherited,overloaded,isShorthand);this._parentPane=stylesPane;this.isShorthand=isShorthand;this._applyStyleThrottler=new WebInspector.Throttler(0);}
WebInspector.StylePropertyTreeElement.Context;WebInspector.StylePropertyTreeElement.prototype={node:function()
{return this._parentPane._node;},editablePane:function()
{return this._parentPane;},parentPane:function()
{return this._parentPane;},section:function()
{return this.treeOutline&&this.treeOutline.section;},_updatePane:function()
{var section=this.section();if(section&&section._parentPane)
section._parentPane._refreshUpdate(section);},_applyNewStyle:function(newStyle)
{var oldStyleRange=(this.style().range);var newStyleRange=(newStyle.range);this._styleRule.updateStyleDeclaration(newStyle);if(this._styleRule.rule())
this._parentPane._styleSheetRuleEdited((this._styleRule.rule()),oldStyleRange,newStyleRange);},toggleEnabled:function(event)
{var disabled=!event.target.checked;function callback(newStyle)
{delete this._parentPane._userOperation;if(!newStyle)
return;this._applyNewStyle(newStyle);var section=this.section();this._updatePane();this.styleTextAppliedForTest();}
this._parentPane._userOperation=true;this.property.setDisabled(disabled,callback.bind(this));event.consume();},onpopulate:function()
{if(this.children.length||!this.isShorthand)
return;var longhandProperties=this.style().longhandProperties(this.name);for(var i=0;i<longhandProperties.length;++i){var name=longhandProperties[i].name;var inherited=false;var overloaded=false;var section=this.section();if(section){inherited=section.isPropertyInherited(name);overloaded=section.styleRule.isPropertyOverloaded(name);}
var liveProperty=this.style().getLiveProperty(name);if(!liveProperty)
continue;var item=new WebInspector.StylePropertyTreeElement(this._parentPane,this._styleRule,liveProperty,false,inherited,overloaded);this.appendChild(item);}},onattach:function()
{WebInspector.StylePropertyTreeElementBase.prototype.onattach.call(this);this.listItemElement.addEventListener("mousedown",this._mouseDown.bind(this));this.listItemElement.addEventListener("mouseup",this._resetMouseDownElement.bind(this));this.listItemElement.addEventListener("click",this._mouseClick.bind(this));},_mouseDown:function(event)
{if(this._parentPane){this._parentPane._mouseDownTreeElement=this;this._parentPane._mouseDownTreeElementIsName=this._isNameElement((event.target));this._parentPane._mouseDownTreeElementIsValue=this._isValueElement((event.target));}},_resetMouseDownElement:function()
{if(this._parentPane){delete this._parentPane._mouseDownTreeElement;delete this._parentPane._mouseDownTreeElementIsName;delete this._parentPane._mouseDownTreeElementIsValue;}},updateTitle:function()
{WebInspector.StylePropertyTreeElementBase.prototype.updateTitle.call(this);if(this.parsedOk&&this.section()&&this.parent.root){var enabledCheckboxElement=createElement("input");enabledCheckboxElement.className="enabled-button";enabledCheckboxElement.type="checkbox";enabledCheckboxElement.checked=!this.disabled;enabledCheckboxElement.addEventListener("click",this.toggleEnabled.bind(this),false);this.listItemElement.insertBefore(enabledCheckboxElement,this.listItemElement.firstChild);}},_mouseClick:function(event)
{if(!event.target.window().getSelection().isCollapsed)
return;event.consume(true);if(event.target===this.listItemElement){var section=this.section();if(!section||!section.editable)
return;if(section._checkWillCancelEditing())
return;section.addNewBlankProperty(this.property.index+1).startEditing();return;}
if(WebInspector.KeyboardShortcut.eventHasCtrlOrMeta((event))&&this.section().navigable){this._navigateToSource((event.target));return;}
this.startEditing((event.target));},_navigateToSource:function(element)
{console.assert(this.section().navigable);var propertyNameClicked=element===this.nameElement;var uiLocation=WebInspector.cssWorkspaceBinding.propertyUILocation(this.property,propertyNameClicked);if(uiLocation)
WebInspector.Revealer.reveal(uiLocation);},_isNameElement:function(element)
{return element.enclosingNodeOrSelfWithClass("webkit-css-property")===this.nameElement;},_isValueElement:function(element)
{return!!element.enclosingNodeOrSelfWithClass("value");},startEditing:function(selectElement)
{if(this.parent.isShorthand)
return;if(selectElement===this._expandElement)
return;var section=this.section();if(section&&!section.editable)
return;if(!selectElement)
selectElement=this.nameElement;else
selectElement=selectElement.enclosingNodeOrSelfWithClass("webkit-css-property")||selectElement.enclosingNodeOrSelfWithClass("value");if(WebInspector.isBeingEdited(selectElement))
return;var isEditingName=selectElement===this.nameElement;if(!isEditingName)
this.valueElement.textContent=restoreURLs(this.valueElement.textContent,this.value);function restoreURLs(fieldValue,modelValue)
{const urlRegex=/\b(url\([^)]*\))/g;var splitFieldValue=fieldValue.split(urlRegex);if(splitFieldValue.length===1)
return fieldValue;var modelUrlRegex=new RegExp(urlRegex);for(var i=1;i<splitFieldValue.length;i+=2){var match=modelUrlRegex.exec(modelValue);if(match)
splitFieldValue[i]=match[0];}
return splitFieldValue.join("");}
var context={expanded:this.expanded,hasChildren:this.hasChildren,isEditingName:isEditingName,previousContent:selectElement.textContent};this.hasChildren=false;if(selectElement.parentElement)
selectElement.parentElement.classList.add("child-editing");selectElement.textContent=selectElement.textContent;function pasteHandler(context,event)
{var data=event.clipboardData.getData("Text");if(!data)
return;var colonIdx=data.indexOf(":");if(colonIdx<0)
return;var name=data.substring(0,colonIdx).trim();var value=data.substring(colonIdx+1).trim();event.preventDefault();if(!("originalName"in context)){context.originalName=this.nameElement.textContent;context.originalValue=this.valueElement.textContent;}
this.property.name=name;this.property.value=value;this.nameElement.textContent=name;this.valueElement.textContent=value;this.nameElement.normalize();this.valueElement.normalize();this.editingCommitted(event.target.textContent,context,"forward");}
function blurListener(context,event)
{var treeElement=this._parentPane._mouseDownTreeElement;var moveDirection="";if(treeElement===this){if(isEditingName&&this._parentPane._mouseDownTreeElementIsValue)
moveDirection="forward";if(!isEditingName&&this._parentPane._mouseDownTreeElementIsName)
moveDirection="backward";}
this.editingCommitted(event.target.textContent,context,moveDirection);}
delete this.originalPropertyText;this._parentPane._isEditingStyle=true;if(selectElement.parentElement)
selectElement.parentElement.scrollIntoViewIfNeeded(false);var applyItemCallback=!isEditingName?this._applyFreeFlowStyleTextEdit.bind(this):undefined;this._prompt=new WebInspector.StylesSidebarPane.CSSPropertyPrompt(isEditingName?WebInspector.CSSMetadata.cssPropertiesMetainfo:WebInspector.CSSMetadata.keywordsForProperty(this.nameElement.textContent),this,isEditingName);this._prompt.setAutocompletionTimeout(0);if(applyItemCallback){this._prompt.addEventListener(WebInspector.TextPrompt.Events.ItemApplied,applyItemCallback,this);this._prompt.addEventListener(WebInspector.TextPrompt.Events.ItemAccepted,applyItemCallback,this);}
var proxyElement=this._prompt.attachAndStartEditing(selectElement,blurListener.bind(this,context));proxyElement.addEventListener("keydown",this._editingNameValueKeyDown.bind(this,context),false);proxyElement.addEventListener("keypress",this._editingNameValueKeyPress.bind(this,context),false);proxyElement.addEventListener("input",this._editingNameValueInput.bind(this,context),false);if(isEditingName)
proxyElement.addEventListener("paste",pasteHandler.bind(this,context),false);selectElement.window().getSelection().setBaseAndExtent(selectElement,0,selectElement,1);},_editingNameValueKeyDown:function(context,event)
{if(event.handled)
return;var result;if(isEnterKey(event)){event.preventDefault();result="forward";}else if(event.keyCode===WebInspector.KeyboardShortcut.Keys.Esc.code||event.keyIdentifier==="U+001B")
result="cancel";else if(!context.isEditingName&&this._newProperty&&event.keyCode===WebInspector.KeyboardShortcut.Keys.Backspace.code){var selection=event.target.window().getSelection();if(selection.isCollapsed&&!selection.focusOffset){event.preventDefault();result="backward";}}else if(event.keyIdentifier==="U+0009"){result=event.shiftKey?"backward":"forward";event.preventDefault();}
if(result){switch(result){case"cancel":this.editingCancelled(null,context);break;case"forward":case"backward":this.editingCommitted(event.target.textContent,context,result);break;}
event.consume();return;}},_editingNameValueKeyPress:function(context,event)
{function shouldCommitValueSemicolon(text,cursorPosition)
{var openQuote="";for(var i=0;i<cursorPosition;++i){var ch=text[i];if(ch==="\\"&&openQuote!=="")
++i;else if(!openQuote&&(ch==="\""||ch==="'"))
openQuote=ch;else if(openQuote===ch)
openQuote="";}
return!openQuote;}
var keyChar=String.fromCharCode(event.charCode);var isFieldInputTerminated=(context.isEditingName?keyChar===":":keyChar===";"&&shouldCommitValueSemicolon(event.target.textContent,event.target.selectionLeftOffset()));if(isFieldInputTerminated){event.consume(true);this.editingCommitted(event.target.textContent,context,"forward");return;}},_editingNameValueInput:function(context,event)
{if(!context.isEditingName&&(!this._parentPane._node.pseudoType()||this.name!=="content"))
this._applyFreeFlowStyleTextEdit();},_applyFreeFlowStyleTextEdit:function()
{var valueText=this.valueElement.textContent;if(valueText.indexOf(";")===-1)
this.applyStyleText(this.nameElement.textContent+": "+valueText,false,false,false);},kickFreeFlowStyleEditForTest:function()
{this._applyFreeFlowStyleTextEdit();},editingEnded:function(context)
{this._resetMouseDownElement();this.hasChildren=context.hasChildren;if(context.expanded)
this.expand();var editedElement=context.isEditingName?this.nameElement:this.valueElement;if(editedElement.parentElement)
editedElement.parentElement.classList.remove("child-editing");delete this._parentPane._isEditingStyle;},editingCancelled:function(element,context)
{this._removePrompt();this._revertStyleUponEditingCanceled(this.originalPropertyText);this.editingEnded(context);},_revertStyleUponEditingCanceled:function(originalPropertyText)
{if(typeof originalPropertyText==="string"){delete this.originalPropertyText;this.applyStyleText(originalPropertyText,true,false,true);}else{if(this._newProperty)
this.treeOutline.removeChild(this);else
this.updateTitle();}},_findSibling:function(moveDirection)
{var target=this;do{target=(moveDirection==="forward"?target.nextSibling:target.previousSibling);}while(target&&target.inherited);return target;},editingCommitted:function(userInput,context,moveDirection)
{this._removePrompt();this.editingEnded(context);var isEditingName=context.isEditingName;var createNewProperty,moveToPropertyName,moveToSelector;var isDataPasted="originalName"in context;var isDirtyViaPaste=isDataPasted&&(this.nameElement.textContent!==context.originalName||this.valueElement.textContent!==context.originalValue);var isPropertySplitPaste=isDataPasted&&isEditingName&&this.valueElement.textContent!==context.originalValue;var moveTo=this;var moveToOther=(isEditingName^(moveDirection==="forward"));var abandonNewProperty=this._newProperty&&!userInput&&(moveToOther||isEditingName);if(moveDirection==="forward"&&(!isEditingName||isPropertySplitPaste)||moveDirection==="backward"&&isEditingName){moveTo=moveTo._findSibling(moveDirection);if(moveTo)
moveToPropertyName=moveTo.name;else if(moveDirection==="forward"&&(!this._newProperty||userInput))
createNewProperty=true;else if(moveDirection==="backward")
moveToSelector=true;}
var moveToIndex=moveTo&&this.treeOutline?this.treeOutline.children.indexOf(moveTo):-1;var blankInput=/^\s*$/.test(userInput);var shouldCommitNewProperty=this._newProperty&&(isPropertySplitPaste||moveToOther||(!moveDirection&&!isEditingName)||(isEditingName&&blankInput));var section=(this.section());if(((userInput!==context.previousContent||isDirtyViaPaste)&&!this._newProperty)||shouldCommitNewProperty){section._afterUpdate=moveToNextCallback.bind(this,this._newProperty,!blankInput,section);var propertyText;if(blankInput||(this._newProperty&&/^\s*$/.test(this.valueElement.textContent)))
propertyText="";else{if(isEditingName)
propertyText=userInput+": "+this.property.value;else
propertyText=this.property.name+": "+userInput;}
this.applyStyleText(propertyText,true,true,false);}else{if(isEditingName)
this.property.name=userInput;else
this.property.value=userInput;if(!isDataPasted&&!this._newProperty)
this.updateTitle();moveToNextCallback.call(this,this._newProperty,false,section);}
function moveToNextCallback(alreadyNew,valueChanged,section)
{if(!moveDirection)
return;if(moveTo&&moveTo.parent){moveTo.startEditing(!isEditingName?moveTo.nameElement:moveTo.valueElement);return;}
if(moveTo&&!moveTo.parent){var propertyElements=section.propertiesTreeOutline.children;if(moveDirection==="forward"&&blankInput&&!isEditingName)
--moveToIndex;if(moveToIndex>=propertyElements.length&&!this._newProperty)
createNewProperty=true;else{var treeElement=moveToIndex>=0?propertyElements[moveToIndex]:null;if(treeElement){var elementToEdit=!isEditingName||isPropertySplitPaste?treeElement.nameElement:treeElement.valueElement;if(alreadyNew&&blankInput)
elementToEdit=moveDirection==="forward"?treeElement.nameElement:treeElement.valueElement;treeElement.startEditing(elementToEdit);return;}else if(!alreadyNew)
moveToSelector=true;}}
if(createNewProperty){if(alreadyNew&&!valueChanged&&(isEditingName^(moveDirection==="backward")))
return;section.addNewBlankProperty().startEditing();return;}
if(abandonNewProperty){moveTo=this._findSibling(moveDirection);var sectionToEdit=(moveTo||moveDirection==="backward")?section:section.nextEditableSibling();if(sectionToEdit){if(sectionToEdit.rule())
sectionToEdit.startEditingSelector();else
sectionToEdit._moveEditorFromSelector(moveDirection);}
return;}
if(moveToSelector){if(section.rule())
section.startEditingSelector();else
section._moveEditorFromSelector(moveDirection);}}},_removePrompt:function()
{if(this._prompt){this._prompt.detach();delete this._prompt;}},_hasBeenModifiedIncrementally:function()
{return typeof this.originalPropertyText==="string"||(!!this.property.propertyText&&this._newProperty);},styleTextAppliedForTest:function()
{},applyStyleText:function(styleText,updateInterface,majorChange,isRevert)
{this._applyStyleThrottler.schedule(this._innerApplyStyleText.bind(this,styleText,updateInterface,majorChange,isRevert));},_innerApplyStyleText:function(styleText,updateInterface,majorChange,isRevert,finishedCallback)
{function userOperationFinishedCallback(parentPane,updateInterface)
{if(updateInterface)
delete parentPane._userOperation;finishedCallback();}
if(!isRevert&&!updateInterface&&!this._hasBeenModifiedIncrementally()){this.originalPropertyText=this.property.propertyText;}
if(!this.treeOutline)
return;var section=this.section();styleText=styleText.replace(/\s/g," ").trim();var styleTextLength=styleText.length;if(!styleTextLength&&updateInterface&&!isRevert&&this._newProperty&&!this._hasBeenModifiedIncrementally()){this.parent.removeChild(this);section.afterUpdate();return;}
var currentNode=this._parentPane._node;if(updateInterface)
this._parentPane._userOperation=true;function callback(userCallback,originalPropertyText,newStyle)
{if(!newStyle){if(updateInterface){this._revertStyleUponEditingCanceled(originalPropertyText);}
userCallback();this.styleTextAppliedForTest();return;}
this._applyNewStyle(newStyle);if(this._newProperty)
this._newPropertyInStyle=true;this.property=newStyle.propertyAt(this.property.index);if(updateInterface&&currentNode===this.node())
this._updatePane();userCallback();this.styleTextAppliedForTest();}
if(styleText.length&&!/;\s*$/.test(styleText))
styleText+=";";var overwriteProperty=!!(!this._newProperty||this._newPropertyInStyle);var boundCallback=callback.bind(this,userOperationFinishedCallback.bind(null,this._parentPane,updateInterface),this.originalPropertyText);if(overwriteProperty&&styleText===this.property.propertyText)
boundCallback.call(null,this.property.ownerStyle)
else
this.property.setText(styleText,majorChange,overwriteProperty,boundCallback);},ondblclick:function()
{return true;},isEventWithinDisclosureTriangle:function(event)
{return event.target===this._expandElement;},__proto__:WebInspector.StylePropertyTreeElementBase.prototype}
WebInspector.StylesSidebarPane.CSSPropertyPrompt=function(cssCompletions,sidebarPane,isEditingName)
{WebInspector.TextPrompt.call(this,this._buildPropertyCompletions.bind(this),WebInspector.StyleValueDelimiters);this.setSuggestBoxEnabled(true);this._cssCompletions=cssCompletions;this._sidebarPane=sidebarPane;this._isEditingName=isEditingName;if(!isEditingName)
this.disableDefaultSuggestionForEmptyInput();}
WebInspector.StylesSidebarPane.CSSPropertyPrompt.prototype={onKeyDown:function(event)
{switch(event.keyIdentifier){case"Up":case"Down":case"PageUp":case"PageDown":if(this._handleNameOrValueUpDown(event)){event.preventDefault();return;}
break;case"Enter":if(this.autoCompleteElement&&!this.autoCompleteElement.textContent.length){this.tabKeyPressed();return;}
break;}
WebInspector.TextPrompt.prototype.onKeyDown.call(this,event);},onMouseWheel:function(event)
{if(this._handleNameOrValueUpDown(event)){event.consume(true);return;}
WebInspector.TextPrompt.prototype.onMouseWheel.call(this,event);},tabKeyPressed:function()
{this.acceptAutoComplete();return false;},_handleNameOrValueUpDown:function(event)
{function finishHandler(originalValue,replacementString)
{this._sidebarPane.applyStyleText(this._sidebarPane.nameElement.textContent+": "+this._sidebarPane.valueElement.textContent,false,false,false);}
function customNumberHandler(prefix,number,suffix)
{if(number!==0&&!suffix.length&&WebInspector.CSSMetadata.isLengthProperty(this._sidebarPane.property.name))
suffix="px";return prefix+number+suffix;}
if(!this._isEditingName&&WebInspector.handleElementValueModifications(event,this._sidebarPane.valueElement,finishHandler.bind(this),this._isValueSuggestion.bind(this),customNumberHandler.bind(this)))
return true;return false;},_isValueSuggestion:function(word)
{if(!word)
return false;word=word.toLowerCase();return this._cssCompletions.keySet().hasOwnProperty(word);},_buildPropertyCompletions:function(proxyElement,wordRange,force,completionsReadyCallback)
{var prefix=wordRange.toString().toLowerCase();if(!prefix&&!force&&(this._isEditingName||proxyElement.textContent.length)){completionsReadyCallback([]);return;}
var results=this._cssCompletions.startsWith(prefix);var userEnteredText=wordRange.toString().replace("-","");if(userEnteredText&&(userEnteredText===userEnteredText.toUpperCase())){for(var i=0;i<results.length;++i)
results[i]=results[i].toUpperCase();}
var selectedIndex=this._cssCompletions.mostUsedOf(results);completionsReadyCallback(results,selectedIndex);},__proto__:WebInspector.TextPrompt.prototype}
WebInspector.StylesSidebarPane.MatchedRulesPayload=function()
{this.inlineStyle=null;this.attributesStyle=null;this.matchedCSSRules=null;this.pseudoElements=null;this.inherited=null;}
WebInspector.StylesSidebarPane.MatchedRulesPayload.prototype={fulfilled:function()
{return!!(this.matchedCSSRules&&this.pseudoElements&&this.inherited);}};WebInspector.ElementsPanel=function()
{WebInspector.Panel.call(this,"elements");this.registerRequiredCSS("elements/elementsPanel.css");this._splitView=new WebInspector.SplitView(true,true,"elementsPanelSplitViewState",325,325);this._splitView.addEventListener(WebInspector.SplitView.Events.SidebarSizeChanged,this._updateTreeOutlineVisibleWidth.bind(this));this._splitView.show(this.element);this._searchableView=new WebInspector.SearchableView(this);this._searchableView.setMinimumSize(25,19);this._splitView.setMainView(this._searchableView);var stackElement=this._searchableView.element;this._contentElement=stackElement.createChild("div");this._contentElement.id="elements-content";if(WebInspector.settings.domWordWrap.get())
this._contentElement.classList.add("elements-wrap");WebInspector.settings.domWordWrap.addChangeListener(this._domWordWrapSettingChanged.bind(this));var crumbsContainer=stackElement.createChild("div");crumbsContainer.id="elements-crumbs";this._breadcrumbs=new WebInspector.ElementsBreadcrumbs();this._breadcrumbs.show(crumbsContainer);this._breadcrumbs.addEventListener(WebInspector.ElementsBreadcrumbs.Events.NodeSelected,this._crumbNodeSelected,this);this.sidebarPanes={};this.sidebarPanes.platformFonts=new WebInspector.PlatformFontsSidebarPane();this.sidebarPanes.computedStyle=new WebInspector.ComputedStyleSidebarPane();this.sidebarPanes.styles=new WebInspector.StylesSidebarPane(this.sidebarPanes.computedStyle,this._setPseudoClassForNode.bind(this));this.sidebarPanes.styles.addEventListener(WebInspector.StylesSidebarPane.Events.SelectorEditingStarted,this._onEditingSelectorStarted.bind(this));this.sidebarPanes.styles.addEventListener(WebInspector.StylesSidebarPane.Events.SelectorEditingEnded,this._onEditingSelectorEnded.bind(this));this._matchedStylesFilterBoxContainer=createElement("div");this._matchedStylesFilterBoxContainer.className="sidebar-pane-filter-box";this.sidebarPanes.styles.setFilterBoxContainer(this._matchedStylesFilterBoxContainer);this._computedStylesFilterBoxContainer=createElement("div");this._computedStylesFilterBoxContainer.className="sidebar-pane-filter-box";this.sidebarPanes.computedStyle.setFilterBoxContainer(this._computedStylesFilterBoxContainer);this.sidebarPanes.metrics=new WebInspector.MetricsSidebarPane();this.sidebarPanes.properties=new WebInspector.PropertiesSidebarPane();this.sidebarPanes.domBreakpoints=WebInspector.domBreakpointsSidebarPane.createProxy(this);this.sidebarPanes.eventListeners=new WebInspector.EventListenersSidebarPane();this.sidebarPanes.animations=new WebInspector.AnimationsSidebarPane(this.sidebarPanes.styles);WebInspector.dockController.addEventListener(WebInspector.DockController.Events.DockSideChanged,this._dockSideChanged.bind(this));WebInspector.settings.splitVerticallyWhenDockedToRight.addChangeListener(this._dockSideChanged.bind(this));this._dockSideChanged();this._treeOutlines=[];this._targetToTreeOutline=new Map();WebInspector.targetManager.observeTargets(this);WebInspector.settings.showUAShadowDOM.addChangeListener(this._showUAShadowDOMChanged.bind(this));WebInspector.targetManager.addModelListener(WebInspector.DOMModel,WebInspector.DOMModel.Events.DocumentUpdated,this._documentUpdatedEvent,this);WebInspector.targetManager.addModelListener(WebInspector.CSSStyleModel,WebInspector.CSSStyleModel.Events.ModelWasEnabled,this._updateSidebars,this);WebInspector.extensionServer.addEventListener(WebInspector.ExtensionServer.Events.SidebarPaneAdded,this._extensionSidebarPaneAdded,this);}
WebInspector.ElementsPanel.prototype={_onEditingSelectorStarted:function()
{for(var i=0;i<this._treeOutlines.length;++i)
this._treeOutlines[i].setPickNodeMode(true);},_onEditingSelectorEnded:function()
{for(var i=0;i<this._treeOutlines.length;++i)
this._treeOutlines[i].setPickNodeMode(false);},targetAdded:function(target)
{var treeOutline=new WebInspector.ElementsTreeOutline(target,true,true,this._setPseudoClassForNode.bind(this));treeOutline.setWordWrap(WebInspector.settings.domWordWrap.get());treeOutline.wireToDOMModel();treeOutline.addEventListener(WebInspector.ElementsTreeOutline.Events.SelectedNodeChanged,this._selectedNodeChanged,this);treeOutline.addEventListener(WebInspector.ElementsTreeOutline.Events.NodePicked,this._onNodePicked,this);treeOutline.addEventListener(WebInspector.ElementsTreeOutline.Events.ElementsTreeUpdated,this._updateBreadcrumbIfNeeded,this);this._treeOutlines.push(treeOutline);this._targetToTreeOutline.set(target,treeOutline);if(this.isShowing())
this.wasShown();},targetRemoved:function(target)
{var treeOutline=this._targetToTreeOutline.remove(target);treeOutline.unwireFromDOMModel();this._treeOutlines.remove(treeOutline);treeOutline.element.remove();},_firstTreeOutlineDeprecated:function()
{return this._treeOutlines[0]||null;},_updateTreeOutlineVisibleWidth:function()
{if(!this._treeOutlines.length)
return;var width=this._splitView.element.offsetWidth;if(this._splitView.isVertical())
width-=this._splitView.sidebarSize();for(var i=0;i<this._treeOutlines.length;++i){this._treeOutlines[i].setVisibleWidth(width);this._treeOutlines[i].updateSelection();}
this._breadcrumbs.updateSizes();},defaultFocusedElement:function()
{return this._treeOutlines.length?this._treeOutlines[0].element:this.element;},searchableView:function()
{return this._searchableView;},wasShown:function()
{for(var i=0;i<this._treeOutlines.length;++i){var treeOutline=this._treeOutlines[i];if(treeOutline.element.parentElement!==this._contentElement)
this._contentElement.appendChild(treeOutline.element);}
WebInspector.Panel.prototype.wasShown.call(this);this._breadcrumbs.update();for(var i=0;i<this._treeOutlines.length;++i){var treeOutline=this._treeOutlines[i];treeOutline.updateSelection();treeOutline.setVisible(true);if(!treeOutline.rootDOMNode)
if(treeOutline.domModel().existingDocument())
this._documentUpdated(treeOutline.domModel(),treeOutline.domModel().existingDocument());else
treeOutline.domModel().requestDocument();}},willHide:function()
{for(var i=0;i<this._treeOutlines.length;++i){var treeOutline=this._treeOutlines[i];treeOutline.domModel().hideDOMNodeHighlight();treeOutline.setVisible(false);this._contentElement.removeChild(treeOutline.element);}
if(this._popoverHelper)
this._popoverHelper.hidePopover();WebInspector.Panel.prototype.willHide.call(this);},onResize:function()
{this._updateTreeOutlineVisibleWidth();},_setPseudoClassForNode:function(node,pseudoClass,enable)
{if(!node||!node.target().cssModel.forcePseudoState(node,pseudoClass,enable))
return;this._treeOutlineForNode(node).updateOpenCloseTags(node);this._updateCSSSidebars();WebInspector.notifications.dispatchEventToListeners(WebInspector.UserMetrics.UserAction,{action:WebInspector.UserMetrics.UserActionNames.ForcedElementState,selector:WebInspector.DOMPresentationUtils.fullQualifiedSelector(node,false),enabled:enable,state:pseudoClass});},_onNodePicked:function(event)
{if(!this.sidebarPanes.styles.isEditingSelector())
return;this.sidebarPanes.styles.updateEditingSelectorForNode((event.data));},_selectedNodeChanged:function(event)
{var selectedNode=(event.data);for(var i=0;i<this._treeOutlines.length;++i){if(!selectedNode||selectedNode.domModel()!==this._treeOutlines[i].domModel())
this._treeOutlines[i].selectDOMNode(null);}
if(!selectedNode&&this._lastValidSelectedNode)
this._selectedPathOnReset=this._lastValidSelectedNode.path();this._breadcrumbs.setSelectedNode(selectedNode);this._updateSidebars();if(selectedNode){ConsoleAgent.addInspectedNode(selectedNode.id);this._lastValidSelectedNode=selectedNode;}
WebInspector.notifications.dispatchEventToListeners(WebInspector.NotificationService.Events.SelectedNodeChanged);},_updateSidebars:function()
{this._updateCSSSidebars();this.sidebarPanes.properties.setNode(this.selectedDOMNode());this.sidebarPanes.eventListeners.setNode(this.selectedDOMNode());this.sidebarPanes.animations.setNode(this.selectedDOMNode());},_reset:function()
{delete this.currentQuery;},_documentUpdatedEvent:function(event)
{this._documentUpdated((event.target),(event.data));},_documentUpdated:function(domModel,inspectedRootDocument)
{this._reset();this.searchCanceled();var treeOutline=this._targetToTreeOutline.get(domModel.target());treeOutline.rootDOMNode=inspectedRootDocument;if(!inspectedRootDocument){if(this.isShowing())
domModel.requestDocument();return;}
WebInspector.domBreakpointsSidebarPane.restoreBreakpoints(domModel.target());function selectNode(candidateFocusNode)
{if(!candidateFocusNode)
candidateFocusNode=inspectedRootDocument.body||inspectedRootDocument.documentElement;if(!candidateFocusNode)
return;this.selectDOMNode(candidateFocusNode);if(treeOutline.selectedTreeElement)
treeOutline.selectedTreeElement.expand();}
function selectLastSelectedNode(nodeId)
{if(this.selectedDOMNode()){return;}
var node=nodeId?domModel.nodeForId(nodeId):null;selectNode.call(this,node);}
if(this._omitDefaultSelection)
return;if(this._selectedPathOnReset)
domModel.pushNodeByPathToFrontend(this._selectedPathOnReset,selectLastSelectedNode.bind(this));else
selectNode.call(this,null);delete this._selectedPathOnReset;},searchCanceled:function()
{delete this._searchQuery;this._hideSearchHighlights();this._searchableView.updateSearchMatchesCount(0);delete this._currentSearchResultIndex;delete this._searchResults;var targets=WebInspector.targetManager.targets();for(var i=0;i<targets.length;++i)
targets[i].domModel.cancelSearch();},performSearch:function(searchConfig,shouldJump,jumpBackwards)
{var query=searchConfig.query;this.searchCanceled();const whitespaceTrimmedQuery=query.trim();if(!whitespaceTrimmedQuery.length)
return;this._searchQuery=query;var targets=WebInspector.targetManager.targets();var promises=[];for(var i=0;i<targets.length;++i)
promises.push(targets[i].domModel.performSearchPromise(whitespaceTrimmedQuery,WebInspector.settings.showUAShadowDOM.get()));Promise.all(promises).then(resultCountCallback.bind(this));function resultCountCallback(resultCounts)
{this._searchResults=[];for(var i=0;i<resultCounts.length;++i){var resultCount=resultCounts[i];for(var j=0;j<resultCount;++j)
this._searchResults.push({target:targets[i],index:j,node:undefined});}
this._searchableView.updateSearchMatchesCount(this._searchResults.length);if(!this._searchResults.length)
return;this._currentSearchResultIndex=-1;if(shouldJump)
this._jumpToSearchResult(jumpBackwards?-1:0);}},_domWordWrapSettingChanged:function(event)
{this._contentElement.classList.toggle("elements-wrap",event.data);for(var i=0;i<this._treeOutlines.length;++i)
this._treeOutlines[i].setWordWrap((event.data));var selectedNode=this.selectedDOMNode();if(!selectedNode)
return;var treeElement=this._treeElementForNode(selectedNode);if(treeElement)
treeElement.updateSelection();},switchToAndFocus:function(node)
{this._searchableView.cancelSearch();WebInspector.inspectorView.setCurrentPanel(this);this.selectDOMNode(node,true);},_getPopoverAnchor:function(element,event)
{var anchor=element.enclosingNodeOrSelfWithClass("webkit-html-resource-link");if(!anchor||!anchor.href)
return;return anchor;},_showPopover:function(anchor,popover)
{var node=this.selectedDOMNode();if(node)
WebInspector.DOMPresentationUtils.buildImagePreviewContents(node.target(),anchor.href,true,showPopover);function showPopover(contents)
{if(!contents)
return;popover.setCanShrink(false);popover.showForAnchor(contents,anchor);}},_jumpToSearchResult:function(index)
{this._hideSearchHighlights();this._currentSearchResultIndex=(index+this._searchResults.length)%this._searchResults.length;this._highlightCurrentSearchResult();},jumpToNextSearchResult:function()
{if(!this._searchResults)
return;this._jumpToSearchResult(this._currentSearchResultIndex+1);},jumpToPreviousSearchResult:function()
{if(!this._searchResults)
return;this._jumpToSearchResult(this._currentSearchResultIndex-1);},supportsCaseSensitiveSearch:function()
{return false;},supportsRegexSearch:function()
{return false;},_highlightCurrentSearchResult:function()
{var index=this._currentSearchResultIndex;var searchResults=this._searchResults;var searchResult=searchResults[index];if(searchResult.node===null){this._searchableView.updateCurrentMatchIndex(index);return;}
function searchCallback(node)
{searchResult.node=node;this._highlightCurrentSearchResult();}
if(typeof searchResult.node==="undefined"){searchResult.target.domModel.searchResult(searchResult.index,searchCallback.bind(this));return;}
this._searchableView.updateCurrentMatchIndex(index);var treeElement=this._treeElementForNode(searchResult.node);if(treeElement){treeElement.highlightSearchResults(this._searchQuery);treeElement.reveal();var matches=treeElement.listItemElement.getElementsByClassName("highlighted-search-result");if(matches.length)
matches[0].scrollIntoViewIfNeeded();}},_hideSearchHighlights:function()
{if(!this._searchResults||!this._searchResults.length||this._currentSearchResultIndex<0)
return;var searchResult=this._searchResults[this._currentSearchResultIndex];if(!searchResult.node)
return;var treeOutline=this._targetToTreeOutline.get(searchResult.target);var treeElement=treeOutline.findTreeElement(searchResult.node);if(treeElement)
treeElement.hideSearchHighlights();},selectedDOMNode:function()
{for(var i=0;i<this._treeOutlines.length;++i){var treeOutline=this._treeOutlines[i];if(treeOutline.selectedDOMNode())
return treeOutline.selectedDOMNode();}
return null;},selectDOMNode:function(node,focus)
{for(var i=0;i<this._treeOutlines.length;++i){var treeOutline=this._treeOutlines[i];if(treeOutline.target()===node.target())
treeOutline.selectDOMNode(node,focus);else
treeOutline.selectDOMNode(null);}},_updateBreadcrumbIfNeeded:function(event)
{var nodes=(event.data);this._breadcrumbs.updateNodes(nodes);},_crumbNodeSelected:function(event)
{var node=(event.data);this.selectDOMNode(node,true);},_updateCSSSidebars:function()
{var selectedDOMNode=this.selectedDOMNode();if(!selectedDOMNode||!selectedDOMNode.target().cssModel.isEnabled())
return;this.sidebarPanes.styles.setNode(selectedDOMNode);this.sidebarPanes.metrics.setNode(selectedDOMNode);this.sidebarPanes.platformFonts.setNode(selectedDOMNode);},handleShortcut:function(event)
{function handleUndoRedo(treeOutline)
{if(WebInspector.KeyboardShortcut.eventHasCtrlOrMeta(event)&&!event.shiftKey&&event.keyIdentifier==="U+005A"){treeOutline.target().domModel.undo(this._updateSidebars.bind(this));event.handled=true;return;}
var isRedoKey=WebInspector.isMac()?event.metaKey&&event.shiftKey&&event.keyIdentifier==="U+005A":event.ctrlKey&&event.keyIdentifier==="U+0059";if(isRedoKey){treeOutline.target().domModel.redo(this._updateSidebars.bind(this));event.handled=true;}}
var treeOutline=null;for(var i=0;i<this._treeOutlines.length;++i){if(this._treeOutlines[i].selectedDOMNode()===this._lastValidSelectedNode)
treeOutline=this._treeOutlines[i];}
if(!treeOutline)
return;if(!treeOutline.editing()){handleUndoRedo.call(this,treeOutline);if(event.handled)
return;}
treeOutline.handleShortcut(event);if(event.handled)
return;WebInspector.Panel.prototype.handleShortcut.call(this,event);},_treeOutlineForNode:function(node)
{if(!node)
return null;return this._targetToTreeOutline.get(node.target())||null;},_treeElementForNode:function(node)
{var treeOutline=this._treeOutlineForNode(node);return(treeOutline.findTreeElement(node));},handleCopyEvent:function(event)
{if(!WebInspector.currentFocusElement()||!WebInspector.currentFocusElement().enclosingNodeOrSelfWithClass("elements-tree-outline"))
return;var treeOutline=this._treeOutlineForNode(this.selectedDOMNode());if(treeOutline)
treeOutline.handleCopyOrCutKeyboardEvent(false,event);},handleCutEvent:function(event)
{var treeOutline=this._treeOutlineForNode(this.selectedDOMNode());if(treeOutline)
treeOutline.handleCopyOrCutKeyboardEvent(true,event);},handlePasteEvent:function(event)
{var treeOutline=this._treeOutlineForNode(this.selectedDOMNode());if(treeOutline)
treeOutline.handlePasteKeyboardEvent(event);},_leaveUserAgentShadowDOM:function(node)
{var userAgentShadowRoot=node.ancestorUserAgentShadowRoot();return userAgentShadowRoot?(userAgentShadowRoot.parentNode):node;},revealAndSelectNode:function(node)
{if(WebInspector.inspectElementModeController&&WebInspector.inspectElementModeController.enabled()){InspectorFrontendHost.bringToFront();WebInspector.inspectElementModeController.disable();}
this._omitDefaultSelection=true;WebInspector.inspectorView.setCurrentPanel(this);node=WebInspector.settings.showUAShadowDOM.get()?node:this._leaveUserAgentShadowDOM(node);node.highlightForTwoSeconds();this.selectDOMNode(node,true);delete this._omitDefaultSelection;if(!this._notFirstInspectElement)
InspectorFrontendHost.inspectElementCompleted();this._notFirstInspectElement=true;},appendApplicableItems:function(event,contextMenu,object)
{if(!(object instanceof WebInspector.RemoteObject&&((object)).isNode())&&!(object instanceof WebInspector.DOMNode)&&!(object instanceof WebInspector.DeferredDOMNode)){return;}
if(object instanceof WebInspector.DOMNode){contextMenu.appendSeparator();WebInspector.domBreakpointsSidebarPane.populateNodeContextMenu(object,contextMenu);}
if(this.element.isAncestor((event.target)))
return;var commandCallback=WebInspector.Revealer.reveal.bind(WebInspector.Revealer,object);contextMenu.appendItem(WebInspector.UIString.capitalize("Reveal in Elements ^panel"),commandCallback);},_sidebarContextMenuEventFired:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.show();},_dockSideChanged:function()
{var vertically=WebInspector.dockController.isVertical()&&WebInspector.settings.splitVerticallyWhenDockedToRight.get();this._splitVertically(vertically);},_showUAShadowDOMChanged:function()
{for(var i=0;i<this._treeOutlines.length;++i)
this._treeOutlines[i].update();},_splitVertically:function(vertically)
{if(this.sidebarPaneView&&vertically===!this._splitView.isVertical())
return;if(this.sidebarPaneView){this.sidebarPaneView.detach();this._splitView.uninstallResizer(this.sidebarPaneView.headerElement());}
this._splitView.setVertical(!vertically);var computedPane=new WebInspector.SidebarPane(WebInspector.UIString("Computed"));computedPane.element.classList.add("composite");computedPane.element.classList.add("fill");var expandComputed=computedPane.expand.bind(computedPane);computedPane.bodyElement.classList.add("metrics-and-computed");this.sidebarPanes.computedStyle.setExpandCallback(expandComputed);var matchedStylePanesWrapper=createElement("div");matchedStylePanesWrapper.className="style-panes-wrapper";var computedStylePanesWrapper=createElement("div");computedStylePanesWrapper.className="style-panes-wrapper";function showMetrics(inComputedStyle)
{if(inComputedStyle)
this.sidebarPanes.metrics.show(computedStylePanesWrapper,this.sidebarPanes.computedStyle.element);else
this.sidebarPanes.metrics.show(matchedStylePanesWrapper);}
function tabSelected(event)
{var tabId=(event.data.tabId);if(tabId===computedPane.title())
showMetrics.call(this,true);else if(tabId===stylesPane.title())
showMetrics.call(this,false);}
this.sidebarPaneView=new WebInspector.SidebarTabbedPane();this.sidebarPaneView.element.addEventListener("contextmenu",this._sidebarContextMenuEventFired.bind(this),false);if(this._popoverHelper)
this._popoverHelper.hidePopover();this._popoverHelper=new WebInspector.PopoverHelper(this.sidebarPaneView.element,this._getPopoverAnchor.bind(this),this._showPopover.bind(this));this._popoverHelper.setTimeout(0);if(vertically){this._splitView.installResizer(this.sidebarPaneView.headerElement());this.sidebarPanes.metrics.setExpandCallback(expandComputed);var compositePane=new WebInspector.SidebarPane(this.sidebarPanes.styles.title());compositePane.element.classList.add("composite");compositePane.element.classList.add("fill");var expandComposite=compositePane.expand.bind(compositePane);var splitView=new WebInspector.SplitView(true,true,"stylesPaneSplitViewState",0.5);splitView.show(compositePane.bodyElement);var vbox1=new WebInspector.VBox();vbox1.element.appendChild(matchedStylePanesWrapper);vbox1.element.appendChild(this._matchedStylesFilterBoxContainer);splitView.setMainView(vbox1);var vbox2=new WebInspector.VBox();vbox2.element.appendChild(computedStylePanesWrapper);vbox2.element.appendChild(this._computedStylesFilterBoxContainer);splitView.setSidebarView(vbox2);this.sidebarPanes.styles.setExpandCallback(expandComposite);computedPane.show(computedStylePanesWrapper);computedPane.setExpandCallback(expandComposite);this.sidebarPaneView.addPane(compositePane);}else{var stylesPane=new WebInspector.SidebarPane(this.sidebarPanes.styles.title());stylesPane.element.classList.add("composite");stylesPane.element.classList.add("fill");var expandStyles=stylesPane.expand.bind(stylesPane);stylesPane.bodyElement.classList.add("metrics-and-styles");stylesPane.bodyElement.appendChild(matchedStylePanesWrapper);computedPane.bodyElement.appendChild(computedStylePanesWrapper);this.sidebarPanes.styles.setExpandCallback(expandStyles);this.sidebarPanes.metrics.setExpandCallback(expandStyles);this.sidebarPaneView.addEventListener(WebInspector.TabbedPane.EventTypes.TabSelected,tabSelected,this);stylesPane.bodyElement.appendChild(this._matchedStylesFilterBoxContainer);computedPane.bodyElement.appendChild(this._computedStylesFilterBoxContainer);this.sidebarPaneView.addPane(stylesPane);this.sidebarPaneView.addPane(computedPane);}
this.sidebarPanes.styles.show(matchedStylePanesWrapper);this.sidebarPanes.computedStyle.show(computedStylePanesWrapper);matchedStylePanesWrapper.appendChild(this.sidebarPanes.styles.titleElement);showMetrics.call(this,vertically);this.sidebarPanes.platformFonts.show(computedStylePanesWrapper);this.sidebarPaneView.addPane(this.sidebarPanes.eventListeners);this.sidebarPaneView.addPane(this.sidebarPanes.domBreakpoints);this.sidebarPaneView.addPane(this.sidebarPanes.properties);if(Runtime.experiments.isEnabled("animationInspection"))
this.sidebarPaneView.addPane(this.sidebarPanes.animations);this._extensionSidebarPanesContainer=this.sidebarPaneView;var extensionSidebarPanes=WebInspector.extensionServer.sidebarPanes();for(var i=0;i<extensionSidebarPanes.length;++i)
this._addExtensionSidebarPane(extensionSidebarPanes[i]);this._splitView.setSidebarView(this.sidebarPaneView);this.sidebarPanes.styles.expand();},_extensionSidebarPaneAdded:function(event)
{var pane=(event.data);this._addExtensionSidebarPane(pane);},_addExtensionSidebarPane:function(pane)
{if(pane.panelName()===this.name){this.setHideOnDetach();this._extensionSidebarPanesContainer.addPane(pane);}},__proto__:WebInspector.Panel.prototype}
WebInspector.ElementsPanel.ContextMenuProvider=function()
{}
WebInspector.ElementsPanel.ContextMenuProvider.prototype={appendApplicableItems:function(event,contextMenu,target)
{WebInspector.ElementsPanel.instance().appendApplicableItems(event,contextMenu,target);}}
WebInspector.ElementsPanel.DOMNodeRevealer=function()
{}
WebInspector.ElementsPanel.DOMNodeRevealer.prototype={reveal:function(node)
{return new Promise(revealPromise);function revealPromise(resolve,reject)
{var panel=WebInspector.ElementsPanel.instance();if(node instanceof WebInspector.DOMNode)
onNodeResolved((node));else if(node instanceof WebInspector.DeferredDOMNode)
((node)).resolve(onNodeResolved);else if(node instanceof WebInspector.RemoteObject)
((node)).pushNodeToFrontend(onNodeResolved);else
reject(new Error("Can't reveal a non-node."));function onNodeResolved(resolvedNode)
{if(resolvedNode){panel.revealAndSelectNode(resolvedNode);resolve(undefined);return;}
reject(new Error("Could not resolve node to reveal."));}}}}
WebInspector.ElementsPanel.show=function()
{WebInspector.inspectorView.setCurrentPanel(WebInspector.ElementsPanel.instance());}
WebInspector.ElementsPanel.instance=function()
{if(!WebInspector.ElementsPanel._instanceObject)
WebInspector.ElementsPanel._instanceObject=new WebInspector.ElementsPanel();return WebInspector.ElementsPanel._instanceObject;}
WebInspector.ElementsPanelFactory=function()
{}
WebInspector.ElementsPanelFactory.prototype={createPanel:function()
{return WebInspector.ElementsPanel.instance();}};WebInspector.AnimationsSidebarPane=function(stylesPane)
{WebInspector.ElementsSidebarPane.call(this,WebInspector.UIString("Animations"));this._stylesPane=stylesPane;this.headerElement=createElementWithClass("div","animationsHeader");this._showSubtreeSetting=WebInspector.settings.createSetting("showSubtreeAnimations",true);this._showSubtreeSetting.addChangeListener(this._showSubtreeSettingChanged.bind(this));this._globalControls=new WebInspector.AnimationsSidebarPane.GlobalAnimationControls(this._showSubtreeSetting);this.headerElement.appendChild(this._globalControls.element);this._emptyElement=createElement("div");this._emptyElement.className="info";this._emptyElement.textContent=WebInspector.UIString("No Animations");this.animationsElement=createElement("div");this.animationsElement.appendChild(this._emptyElement);this._animationSections=[];this.bodyElement.appendChild(this.headerElement);this.bodyElement.appendChild(this.animationsElement);}
WebInspector.AnimationsSidebarPane._showSubtreeAnimationsCheckbox=function(setting)
{if(!WebInspector.AnimationsSidebarPane._showSubtreeAnimationsCheckboxElement){WebInspector.AnimationsSidebarPane._showSubtreeAnimationsCheckboxElement=WebInspector.SettingsUI.createSettingCheckbox(WebInspector.UIString("Show subtree animations"),setting,true);WebInspector.AnimationsSidebarPane._showSubtreeAnimationsCheckboxElement.classList.add("checkbox-with-label");}
return WebInspector.AnimationsSidebarPane._showSubtreeAnimationsCheckboxElement;}
WebInspector.AnimationsSidebarPane.prototype={setNode:function(node)
{WebInspector.ElementsSidebarPane.prototype.setNode.call(this,node);if(!node)
return;this._updateTarget(node.target());},_updateTarget:function(target)
{if(this._target===target)
return;if(this._target)
this._target.animationModel.removeEventListener(WebInspector.AnimationModel.Events.AnimationPlayerCreated,this._animationPlayerCreated,this);this._target=target;this._target.animationModel.addEventListener(WebInspector.AnimationModel.Events.AnimationPlayerCreated,this._animationPlayerCreated,this);},_showSubtreeSettingChanged:function()
{this._forceUpdate=true;this.update();},_animationPlayerCreated:function(event)
{this._addAnimationPlayer((event.data));},_addAnimationPlayer:function(player)
{if(this.animationsElement.hasChildNodes()&&!this._animationSections.length)
this.animationsElement.removeChild(this._emptyElement);var section=new WebInspector.AnimationSection(this,this._stylesPane,player);if(this._animationSections.length<10)
section.expand(true);this._animationSections.push(section);this.animationsElement.appendChild(section.element);if(this._animationSections.length>100)
this._target.animationModel.stopListening();},doUpdate:function(finishCallback)
{function animationPlayersCallback(animationPlayers)
{this.animationsElement.removeChildren();this._animationSections=[];if(!animationPlayers||!animationPlayers.length){this.animationsElement.appendChild(this._emptyElement);finishCallback();return;}
for(var i=0;i<animationPlayers.length;++i)
this._addAnimationPlayer(animationPlayers[i]);finishCallback();}
if(!this.node()){this._globalControls.reset();finishCallback();return;}
if(!this._forceUpdate&&this._selectedNode===this.node()){for(var i=0;i<this._animationSections.length;++i)
this._animationSections[i].updateCurrentTime();finishCallback();return;}
this._forceUpdate=false;this._selectedNode=this.node();this.node().target().animationModel.getAnimationPlayers(this.node().id,this._showSubtreeSetting.get(),animationPlayersCallback.bind(this));this.node().target().animationModel.startListening(this.node().id,this._showSubtreeSetting.get());},__proto__:WebInspector.ElementsSidebarPane.prototype}
WebInspector.AnimationSection=function(parentPane,stylesPane,animationPlayer)
{this._parentPane=parentPane;this._stylesPane=stylesPane;this._propertiesElement=createElement("div");this._keyframesElement=createElement("div");this._setAnimationPlayer(animationPlayer);this._updateThrottler=new WebInspector.Throttler(WebInspector.AnimationSection.updateTimeout);this.element=createElement("div");this.element.appendChild(this._createHeader());this.bodyElement=this.element.createChild("div","animation-section-body");this.bodyElement.appendChild(this._createAnimationControls());this.bodyElement.appendChild(this._propertiesElement);this.bodyElement.appendChild(this._keyframesElement);}
WebInspector.AnimationSection.updateTimeout=100;WebInspector.AnimationSection.prototype={_expanded:function()
{return this.bodyElement.classList.contains("expanded");},_toggle:function()
{this.bodyElement.classList.toggle("expanded");this.updateCurrentTime();},expand:function(expanded)
{this.bodyElement.classList.toggle("expanded",expanded);this.updateCurrentTime();},updateCurrentTime:function()
{if(this._expanded())
this._updateThrottler.schedule(this._updateCurrentTime.bind(this),false);},_updateCurrentTime:function(finishCallback)
{function updateSliderCallback(currentTime,isRunning)
{if(isRunning&&this._parentPane.isShowing()){this.currentTimeSlider.value=this.player.source().iterationCount()==null?currentTime%this.player.source().duration():currentTime;finishCallback();this.updateCurrentTime();}else{this.player.payload().pausedState=true;this._updatePauseButton(true);finishCallback();}}
this.player.getCurrentState(updateSliderCallback.bind(this));},_createCurrentTimeSlider:function()
{function sliderMouseDown()
{this.player.pause(this._setAnimationPlayer.bind(this));this._isPaused=this.player.paused();}
function sliderMouseUp()
{if(this._isPaused)
return;this.player.play(this._setAnimationPlayer.bind(this));this._updatePauseButton(false);this.updateCurrentTime();}
function sliderInputHandler(e)
{this.player.setCurrentTime(parseFloat(e.target.value),this._setAnimationPlayer.bind(this));}
var iterationDuration=this.player.source().duration();var iterationCount=this.player.source().iterationCount();var slider=createElement("input");slider.type="range";slider.min=0;slider.step=0.01;if(!iterationCount){slider.max=iterationDuration;slider.value=this.player.currentTime()%iterationDuration;}else{slider.max=iterationCount*iterationDuration;slider.value=this.player.currentTime();}
slider.addEventListener("input",sliderInputHandler.bind(this));slider.addEventListener("mousedown",sliderMouseDown.bind(this));slider.addEventListener("mouseup",sliderMouseUp.bind(this));this.updateCurrentTime();return slider;},_createHeader:function()
{function nodeResolved(node)
{headerElement.createTextChild(" - ");headerElement.appendChild(WebInspector.DOMPresentationUtils.linkifyNodeReference(node));}
var headerElement=createElementWithClass("div","sidebar-separator");var id=this.player.source().name()?this.player.source().name():this.player.id();headerElement.createTextChild(WebInspector.UIString("Animation")+" "+id);headerElement.addEventListener("click",this._toggle.bind(this),false);this.player.source().getNode(nodeResolved);return headerElement;},_updatePauseButton:function(paused)
{this._pauseButton.setToggled(paused);this._pauseButton.setTitle(paused?WebInspector.UIString("Play animation"):WebInspector.UIString("Pause animation"));},_createAnimationControls:function()
{function pauseButtonHandler()
{if(this.player.paused()){this.player.play(this._setAnimationPlayer.bind(this));this._updatePauseButton(false);this.updateCurrentTime();}else{this.player.pause(this._setAnimationPlayer.bind(this));this._updatePauseButton(true);}}
this._pauseButton=new WebInspector.StatusBarButton("","pause-status-bar-item");this._pauseButton.element.style.display="inline-block";this._updatePauseButton(this.player.paused());this._pauseButton.addEventListener("click",pauseButtonHandler,this);this.currentTimeSlider=this._createCurrentTimeSlider();var controls=createElement("div");var shadowRoot=controls.createShadowRoot();shadowRoot.appendChild(WebInspector.View.createStyleElement("ui/statusBar.css"));shadowRoot.appendChild(this._pauseButton.element);shadowRoot.appendChild(this.currentTimeSlider);return controls;},_setAnimationPlayer:function(p)
{if(!p||p===this.player)
return;this.player=p;this._propertiesElement.removeChildren();var animationObject={"playState":p.playState(),"start-time":p.startTime(),"player-playback-rate":p.playbackRate(),"id":p.id(),"start-delay":p.source().startDelay(),"playback-rate":p.source().playbackRate(),"iteration-start":p.source().iterationStart(),"iteration-count":p.source().iterationCount(),"duration":p.source().duration(),"direction":p.source().direction(),"fill-mode":p.source().fillMode(),"time-fraction":p.source().timeFraction()};var obj=WebInspector.RemoteObject.fromLocalObject(animationObject);var objSection=new WebInspector.ObjectPropertiesSection(obj,WebInspector.UIString("Animation Properties"));this._propertiesElement.appendChild(objSection.element);if(this.player.source().keyframesRule()){var keyframes=this.player.source().keyframesRule().keyframes();for(var j=0;j<keyframes.length;j++){var animationCascade=new WebInspector.SectionCascade();var model=animationCascade.appendModelFromStyle(keyframes[j].style(),keyframes[j].offset());model.setIsAttribute(true);model.setEditable(true);var styleSection=new WebInspector.StylePropertiesSection(this._stylesPane,model);styleSection.expand();this._keyframesElement.appendChild(styleSection.element);}}}}
WebInspector.AnimationsSidebarPane.GlobalPlaybackRates=[0.1,0.25,0.5,1.0];WebInspector.AnimationsSidebarPane.GlobalAnimationControls=function(showSubtreeAnimationsSetting)
{WebInspector.StatusBar.call(this);this.element.classList.add("global-animations-toolbar");var labelElement=createElement("div");labelElement.createTextChild("Global playback:");this.appendStatusBarItem(new WebInspector.StatusBarItemWrapper(labelElement));this._pauseButton=new WebInspector.StatusBarButton("","pause-status-bar-item");this._pauseButton.addEventListener("click",this._pauseHandler.bind(this),this);this.appendStatusBarItem(this._pauseButton);this._playbackRateButtons=[];WebInspector.AnimationsSidebarPane.GlobalPlaybackRates.forEach(this._createPlaybackRateButton.bind(this));var subtreeCheckboxLabel=WebInspector.UIString("Show subtree animations");this._showSubtreeAnimationsCheckbox=new WebInspector.StatusBarCheckbox(subtreeCheckboxLabel,subtreeCheckboxLabel,showSubtreeAnimationsSetting);this.appendStatusBarItem(this._showSubtreeAnimationsCheckbox);this.reset();}
WebInspector.AnimationsSidebarPane.GlobalAnimationControls.prototype={_createPlaybackRateButton:function(playbackRate)
{var button=new WebInspector.StatusBarTextButton(WebInspector.UIString("Set all animations playback rate to "+playbackRate+"x"),"playback-rate-button",playbackRate+"x");button.playbackRate=playbackRate;button.addEventListener("click",this._playbackRateHandler.bind(this,playbackRate),this);this._playbackRateButtons.push(button);this.appendStatusBarItem(button);},reset:function()
{this._paused=false;this._playbackRate=1.0;this._updateControls();},_updateControls:function()
{this._updatePauseButton();for(var i=0;i<this._playbackRateButtons.length;i++)
this._playbackRateButtons[i].setToggled(this._playbackRateButtons[i].playbackRate===this._playbackRate);},_updatePauseButton:function()
{this._pauseButton.setToggled(this._paused);this._pauseButton.setTitle(this._paused?WebInspector.UIString("Resume all animations"):WebInspector.UIString("Pause all animations"));},_pauseHandler:function()
{this._paused=!this._paused;PageAgent.setAnimationsPlaybackRate(this._paused?0:this._playbackRate);this._updatePauseButton();},_playbackRateHandler:function(playbackRate)
{this._playbackRate=playbackRate;this._updateControls();PageAgent.setAnimationsPlaybackRate(this._paused?0:this._playbackRate);},__proto__:WebInspector.StatusBar.prototype};Runtime.cachedResources["elements/breadcrumbs.css"]="/*\n * Copyright 2014 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.crumbs {\n    display: inline-block;\n    pointer-events: auto;\n    cursor: default;\n    font-size: 11px;\n    line-height: 17px;\n}\n\n.crumbs .crumb {\n    display: inline-block;\n    padding: 0 7px;\n    height: 18px;\n    white-space: nowrap;\n}\n\n.crumbs .crumb.collapsed > * {\n    display: none;\n}\n\n.crumbs .crumb.collapsed::before {\n    content: \"\\2026\";\n    font-weight: bold;\n}\n\n.crumbs .crumb.compact .extra {\n    display: none;\n}\n\n.crumbs .crumb.selected, .crumbs .crumb.selected:hover {\n    background-color: rgb(56, 121, 217);\n    color: white;\n    text-shadow: rgba(255, 255, 255, 0.5) 0 0 0;\n}\n\n.crumbs .crumb:hover {\n    background-color: rgb(216, 216, 216);\n}\n\n/*# sourceURL=elements/breadcrumbs.css */";Runtime.cachedResources["elements/elementsPanel.css"]="/*\n * Copyright (C) 2006, 2007, 2008 Apple Inc.  All rights reserved.\n * Copyright (C) 2009 Anthony Ricaud <rik@webkit.org>\n *\n * Redistribution and use in source and binary forms, with or without\n * modification, are permitted provided that the following conditions\n * are met:\n *\n * 1.  Redistributions of source code must retain the above copyright\n *     notice, this list of conditions and the following disclaimer.\n * 2.  Redistributions in binary form must reproduce the above copyright\n *     notice, this list of conditions and the following disclaimer in the\n *     documentation and/or other materials provided with the distribution.\n * 3.  Neither the name of Apple Computer, Inc. (\"Apple\") nor the names of\n *     its contributors may be used to endorse or promote products derived\n *     from this software without specific prior written permission.\n *\n * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS \"AS IS\" AND ANY\n * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY\n * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF\n * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n */\n\n#elements-content {\n    flex: 1 1;\n    overflow: auto;\n    padding: 2px 0 0 0;\n    transform: translateZ(0);\n}\n\n#elements-content:not(.elements-wrap) > div {\n    display: inline-block;\n    min-width: 100%;\n}\n\n#elements-content.elements-wrap {\n    overflow-x: hidden;\n}\n\n#elements-crumbs {\n    flex: 0 0 19px;\n    background-color: white;\n    border-top: 1px solid #ccc;\n    overflow: hidden;\n    height: 19px;\n    width: 100%;\n}\n\n.metrics {\n    padding: 8px;\n    font-size: 10px;\n    text-align: center;\n    white-space: nowrap;\n}\n\n.metrics .label {\n    position: absolute;\n    font-size: 10px;\n    margin-left: 3px;\n    padding-left: 2px;\n    padding-right: 2px;\n}\n\n.metrics .position {\n    border: 1px rgb(66%, 66%, 66%) dotted;\n    background-color: white;\n    display: inline-block;\n    text-align: center;\n    padding: 3px;\n    margin: 3px;\n}\n\n.metrics .margin {\n    border: 1px dashed;\n    background-color: white;\n    display: inline-block;\n    text-align: center;\n    vertical-align: middle;\n    padding: 3px;\n    margin: 3px;\n}\n\n.metrics .border {\n    border: 1px black solid;\n    background-color: white;\n    display: inline-block;\n    text-align: center;\n    vertical-align: middle;\n    padding: 3px;\n    margin: 3px;\n}\n\n.metrics .padding {\n    border: 1px grey dashed;\n    background-color: white;\n    display: inline-block;\n    text-align: center;\n    vertical-align: middle;\n    padding: 3px;\n    margin: 3px;\n}\n\n.metrics .content {\n    position: static;\n    border: 1px gray solid;\n    background-color: white;\n    display: inline-block;\n    text-align: center;\n    vertical-align: middle;\n    padding: 3px;\n    margin: 3px;\n    min-width: 80px;\n    overflow: visible;\n}\n\n.metrics .content span {\n    display: inline-block;\n}\n\n.metrics .editing {\n    position: relative;\n    z-index: 100;\n    cursor: text;\n}\n\n.metrics .left {\n    display: inline-block;\n    vertical-align: middle;\n}\n\n.metrics .right {\n    display: inline-block;\n    vertical-align: middle;\n}\n\n.metrics .top {\n    display: inline-block;\n}\n\n.metrics .bottom {\n    display: inline-block;\n}\n\n.styles-section {\n    padding: 2px 2px 4px 4px;\n    min-height: 18px;\n    white-space: nowrap;\n    background-origin: padding;\n    background-clip: padding;\n    -webkit-user-select: text;\n    border-bottom: 1px solid rgb(191, 191, 191);\n    position: relative;\n}\n\n.styles-pane .sidebar-separator {\n    border-top: 0 none;\n}\n\n.styles-section.user-rule {\n    display: none;\n}\n\n.show-user-styles .styles-section.user-rule {\n    display: block;\n}\n\n.styles-sidebar-placeholder {\n    height: 16px;\n}\n\n.styles-section.read-only:not(.computed-style) {\n    background-color: #eee;\n}\n\n.styles-section .properties li.not-parsed-ok {\n    margin-left: 0;\n}\n\n.styles-section.computed-style .properties li.not-parsed-ok {\n    margin-left: -6px;\n}\n\n.styles-section .properties li.filter-match,\n.styles-section .simple-selector.filter-match {\n    background-color: rgba(255, 255, 0, 0.5);\n}\n\n.styles-section .properties li.overloaded.filter-match {\n    background-color: rgba(255, 255, 0, 0.25);\n}\n\n.styles-section .properties li.not-parsed-ok .exclamation-mark {\n    display: inline-block;\n    position: relative;\n    width: 11px;\n    height: 10px;\n    margin: 0 7px 0 0;\n    top: 1px;\n    left: -36px; /* outdent to compensate for the top-level property indent */\n    -webkit-user-select: none;\n    cursor: default;\n    z-index: 1;\n}\n\n.styles-section .header {\n    white-space: nowrap;\n    background-origin: padding;\n    background-clip: padding;\n}\n\n.styles-section .header .title {\n    word-wrap: break-word;\n    white-space: normal;\n}\n\n.styles-section .header .title .media-list {\n    color: #888;\n}\n\n.styles-section .header .title .media-list.media-matches .media.editable-media {\n    color: #222;\n}\n\n.styles-section .header .title .media:not(.editing-media),\n.styles-section .header .title .media:not(.editing-media) .subtitle {\n    overflow: hidden;\n}\n\n.styles-section .header .subtitle {\n    color: rgb(85, 85, 85);\n    float: right;\n    margin-left: 5px;\n    max-width: 100%;\n    text-overflow: ellipsis;\n    overflow: hidden;\n    white-space: nowrap;\n}\n\n.styles-section .header .subtitle a {\n    color: inherit;\n}\n\n.styles-section .selector {\n    color: #888;\n}\n\n.styles-section .simple-selector.selector-matches {\n    color: #222;\n}\n\n.styles-section a[data-uncopyable] {\n    display: inline-block;\n}\n\n.styles-section a[data-uncopyable]::before {\n    content: attr(data-uncopyable);\n    text-decoration: underline;\n}\n\n.styles-section .properties {\n    display: none;\n    margin: 0;\n    padding: 2px 4px 0 0;\n    list-style: none;\n    clear: both;\n}\n\n.styles-section.matched-styles .properties {\n    padding-left: 0;\n}\n\n.styles-section.no-affect .properties li {\n    opacity: 0.5;\n}\n\n.styles-section.no-affect .properties li.editing {\n    opacity: 1.0;\n}\n\n.styles-section.expanded .properties {\n    display: block;\n}\n\n.styles-section .properties li {\n    margin-left: 12px;\n    padding-left: 22px;\n    white-space: normal;\n    text-overflow: ellipsis;\n    overflow: hidden;\n    cursor: auto;\n}\n\n.styles-section.computed-style.expanded .properties > li {\n    padding-left: 0;\n}\n\n.styles-section.computed-style.expanded .properties > li .webkit-css-property {\n    margin-left: 0;\n}\n\n.styles-section .properties li .webkit-css-property {\n    margin-left: -22px; /* outdent the first line of longhand properties (in an expanded shorthand) to compensate for the \"padding-left\" shift in .styles-section .properties li */\n}\n\n.styles-section.expanded .properties > li {\n    padding-left: 38px;\n}\n\n.styles-section .properties > li .webkit-css-property {\n    margin-left: -38px; /* outdent the first line of the top-level properties to compensate for the \"padding-left\" shift in .styles-section .properties > li */\n}\n\n.styles-section .properties > li.child-editing {\n    padding-left: 8px;\n}\n\n.styles-section .properties > li.child-editing .webkit-css-property {\n    margin-left: 0;\n}\n\n.styles-section.matched-styles .properties li {\n    margin-left: 0 !important;\n}\n\n.styles-section .properties li.child-editing {\n    word-wrap: break-word !important;\n    white-space: normal !important;\n    padding-left: 0;\n}\n\n.styles-section .properties ol {\n    display: none;\n    margin: 0;\n    -webkit-padding-start: 12px;\n    list-style: none;\n}\n\n.styles-section .properties ol.expanded {\n    display: block;\n}\n\n.styles-section.matched-styles .properties li.parent .expand-element {\n    -webkit-user-select: none;\n    background-image: url(Images/statusbarButtonGlyphs.png);\n    background-size: 320px 144px;\n    margin-right: 2px;\n    margin-left: -6px;\n    opacity: 0.55;\n    width: 8px;\n    height: 10px;\n    display: inline-block;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.styles-section.matched-styles .properties li.parent .expand-element {\n    background-image: url(Images/statusbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.styles-section.matched-styles .properties li.parent .expand-element {\n    background-position: -4px -96px;\n}\n\n.styles-section.matched-styles .properties li.parent.expanded .expand-element {\n    background-position: -20px -96px;\n}\n\n.styles-section .properties li .info {\n    padding-top: 4px;\n    padding-bottom: 3px;\n}\n\n.styles-section.matched-styles:not(.read-only):hover .properties .enabled-button {\n    visibility: visible;\n}\n\n.styles-section.matched-styles:not(.read-only) .properties li.disabled .enabled-button {\n    visibility: visible;\n}\n\n.styles-section .properties .enabled-button {\n    visibility: hidden;\n    float: left;\n    font-size: 10px;\n    margin: 0;\n    vertical-align: top;\n    position: relative;\n    z-index: 1;\n    width: 18px;\n    left: -40px; /* original -2px + (-38px) to compensate for the first line outdent */\n    top: 1px;\n}\n\n.styles-section.matched-styles .properties ol.expanded {\n    margin-left: 16px;\n}\n\n.styles-section .properties .overloaded:not(.has-ignorable-error),\n.styles-section .properties .inactive,\n.styles-section .properties .disabled,\n.styles-section .properties .not-parsed-ok:not(.has-ignorable-error) {\n    text-decoration: line-through;\n}\n\n.styles-section .properties .has-ignorable-error .webkit-css-property {\n    color: inherit;\n}\n\n.styles-section.computed-style .properties .disabled {\n    text-decoration: none;\n    opacity: 0.5;\n}\n\n.styles-section .properties .implicit,\n.styles-section .properties .inherited {\n    opacity: 0.5;\n}\n\n.styles-section .properties .has-ignorable-error {\n    color: gray;\n}\n\n.styles-element-state-pane {\n    overflow: hidden;\n    margin-top: -56px;\n    padding-top: 18px;\n    height: 56px;\n    -webkit-transition: margin-top 0.1s ease-in-out;\n    padding-left: 2px;\n}\n\n.styles-element-state-pane.expanded {\n    border-bottom: 1px solid rgb(189, 189, 189);\n    margin-top: 0;\n}\n\n.styles-element-state-pane > table {\n    width: 100%;\n    border-spacing: 0;\n}\n\n.styles-element-state-pane label {\n    display: flex;\n    margin: 1px;\n}\n\n.styles-animations-controls-pane {\n    overflow: hidden;\n    -webkit-transition: height 0.1s ease-out;\n    height: 0;\n}\n\n.styles-animations-controls-pane > * {\n    margin: 6px 4px;\n}\n\n.styles-animations-controls-pane.expanded {\n    border-bottom: 1px solid rgb(189, 189, 189);\n    height: 56px;\n}\n\n.animations-controls {\n    width: 100%;\n    max-width: 200px;\n    display: flex;\n    align-items: center;\n}\n\n.animations-controls > .status-bar {\n    display: inline-block;\n}\n\n.animations-controls > input {\n    flex-grow: 1;\n    margin-right: 10px;\n}\n\n.animations-controls > .playback-label {\n    width: 35px;\n}\n\n.styles-selector {\n    cursor: text;\n}\n\n.section .event-bars {\n    display: none;\n}\n\n.section.expanded .event-bars {\n    display: block;\n}\n\n.events-pane .section.event-bar {\n    position: relative;\n    margin-left: 10px;\n}\n\n.events-pane .section.event-bar:first-child {\n    margin-top: 1px;\n}\n\n.event-bars .event-bar .header {\n    padding: 0 8px 0 6px;\n    min-height: 16px;\n    opacity: 1.0;\n    white-space: nowrap;\n    background-origin: padding;\n    background-clip: padding;\n}\n\n.event-bars .event-bar .header .title {\n    font-weight: normal;\n    text-shadow: white 0 1px 0;\n}\n\n.event-bars .event-bar .header .subtitle {\n    color: rgba(90, 90, 90, 0.75);\n}\n\n.event-bars .event-bar .header::before {\n    -webkit-user-select: none;\n    background-image: url(Images/statusbarButtonGlyphs.png);\n    background-size: 320px 144px;\n    opacity: 0.5;\n    content: \"a\";\n    color: transparent;\n    text-shadow: none;\n    float: left;\n    width: 8px;\n    margin-right: 4px;\n    margin-top: 2px;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.event-bars .event-bar .header::before {\n    background-image: url(Images/statusbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.event-bars .event-bar .header::before {\n    background-position: -4px -96px;\n}\n\n.event-bars .event-bar.expanded .header::before {\n    background-position: -20px -96px;\n}\n\n.image-preview-container {\n    background: transparent;\n    text-align: center;\n}\n\n.image-preview-container img {\n    margin: 2px auto;\n    max-width: 100px;\n    max-height: 100px;\n    background-image: url(Images/checker.png);\n    -webkit-user-select: text;\n    -webkit-user-drag: auto;\n}\n\n.sidebar-pane.composite {\n    position: absolute;\n}\n\n.sidebar-pane.composite > .body {\n    height: 100%;\n}\n\n.sidebar-pane.composite .metrics {\n    border-bottom: 1px solid rgb(64%, 64%, 64%);\n    height: 206px;\n    display: flex;\n    flex-direction: column;\n    -webkit-align-items: center;\n    -webkit-justify-content: center;\n}\n\n.sidebar-pane .metrics-and-styles,\n.sidebar-pane .metrics-and-computed {\n    display: flex !important;\n    flex-direction: column !important;\n    position: relative;\n}\n\n.sidebar-pane .style-panes-wrapper {\n    transform: translateZ(0);\n    flex: 1;\n    overflow-y: auto;\n    position: relative;\n}\n\n.sidebar-pane.composite .metrics-and-computed .sidebar-pane-toolbar,\n.sidebar-pane.composite .metrics-and-styles .sidebar-pane-toolbar {\n    position: absolute;\n}\n\n.sidebar-pane-filter-box {\n    display: flex;\n    border-top: 1px solid rgb(191, 191, 191);\n    flex-basis: 19px;\n}\n\n.sidebar-pane-filter-box > input {\n    outline: none !important;\n    border: none;\n    width: 100%;\n    margin: 0 4px;\n    background: transparent;\n}\n\n.styles-filter-engaged {\n    background-color: rgba(255, 255, 0, 0.5);\n}\n\n.sidebar-pane.composite .metrics-and-computed .sidebar-pane-toolbar {\n    margin-top: 4px;\n    margin-bottom: -4px;\n    position: relative;\n}\n\n.sidebar-pane.composite .platform-fonts .body {\n    padding: 1ex;\n    -webkit-user-select: text;\n}\n\n.sidebar-pane.composite .platform-fonts .sidebar-separator {\n    border-top: none;\n}\n\n.sidebar-pane.composite .platform-fonts .stats-section {\n    margin-bottom: 5px;\n}\n\n.sidebar-pane.composite .platform-fonts .font-stats-item {\n    padding-left: 1em;\n}\n\n.sidebar-pane.composite .platform-fonts .font-stats-item .delimeter {\n    margin: 0 1ex 0 1ex;\n}\n\n.sidebar-pane.composite .metrics-and-styles .metrics {\n    border-bottom: none;\n}\n\n.sidebar-pane > .body > .split-view {\n    position: absolute;\n    top: 0;\n    bottom: 0;\n    left: 0;\n    right: 0;\n}\n\n.panel.elements .sidebar-pane-toolbar > select {\n    float: right;\n    width: 23px;\n    height: 17px;\n    color: transparent;\n    background-color: transparent;\n    border: none;\n    background-repeat: no-repeat;\n    margin: 1px 0 0 0;\n    padding: 0;\n    border-radius: 0;\n    -webkit-appearance: none;\n}\n\n.panel.elements .sidebar-pane-toolbar > select:hover {\n    background-position: -23px 0;\n}\n\n.panel.elements .sidebar-pane-toolbar > select:active {\n    background-position: -46px 0;\n}\n\n.panel.elements .sidebar-pane-toolbar > select.select-filter {\n    background-image: url(Images/paneFilterButtons.png);\n}\n.panel.elements .sidebar-pane-toolbar > select > option,\n.panel.elements .sidebar-pane-toolbar > select > hr {\n    color: black;\n}\n\n.styles-section:not(.read-only) .properties .webkit-css-property.styles-panel-hovered,\n.styles-section:not(.read-only) .properties .value .styles-panel-hovered,\n.styles-section:not(.read-only) .properties .value.styles-panel-hovered,\n.styles-section:not(.read-only) span.simple-selector.styles-panel-hovered {\n    text-decoration: underline;\n    cursor: default;\n}\n\n.styles-clipboard-only {\n    display: inline-block;\n    width: 0;\n    opacity: 0;\n    pointer-events: none;\n    white-space: pre;\n}\n\nli.child-editing .styles-clipboard-only {\n    display: none;\n}\n\nli.editing .swatch,\nli.editing .enabled-button {\n    display: none !important;\n}\n\n.sidebar-separator {\n    background-color: #ddd;\n    padding: 0 5px;\n    border-top: 1px solid #ccc;\n    border-bottom: 1px solid #ccc;\n    color: rgb(50, 50, 50);\n    white-space: nowrap;\n    text-overflow: ellipsis;\n    overflow: hidden;\n    line-height: 16px;\n}\n\n.swatch-inner {\n    width: 100%;\n    height: 100%;\n    display: inline-block;\n    border: 1px solid rgba(128, 128, 128, 0.6);\n}\n\n.swatch-inner:hover {\n    border: 1px solid rgba(64, 64, 64, 0.8);\n}\n\n.animation-section-body {\n    display: none;\n}\n\n.animation-section-body.expanded {\n    display: block;\n}\n\n.animation-section-body .section {\n    border-bottom: 1px solid rgb(191, 191, 191);\n}\n\n.animationsHeader {\n    padding-top: 23px;\n}\n\n.global-animations-toolbar {\n    position: absolute;\n    top: 0;\n    width: 100%;\n    background-color: #eee;\n    border-bottom: 1px solid rgb(163, 163, 163);\n    padding-left: 10px;\n}\n\nlabel.checkbox-with-label {\n    -webkit-user-select: none;\n}\n\n.events-pane .section:not(:first-of-type) {\n    border-top: 1px solid rgb(231, 231, 231);\n}\n\n.event-bar .header .title {\n    font-weight: normal;\n    word-wrap: break-word;\n    white-space: normal;\n    line-height: 18px;\n}\n\n.events-pane .section {\n    margin: 0;\n}\n\n.event-bar .event-properties {\n    padding-left: 16px;\n}\n\n/*# sourceURL=elements/elementsPanel.css */";Runtime.cachedResources["elements/elementsTreeOutline.css"]="/*\n * Copyright (c) 2014 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.outline-disclosure {\n    width: 100%;\n    display: inline-block;\n    line-height: normal;\n}\n\n.outline-disclosure li {\n    padding: 0 0 0 14px;\n    margin-top: 1px;\n    margin-left: -2px;\n    word-wrap: break-word;\n}\n\n.outline-disclosure li.parent {\n    margin-left: -13px;\n}\n\n.outline-disclosure li.parent::before {\n    float: left;\n    width: 10px;\n    box-sizing: border-box;\n}\n\n.outline-disclosure li.parent::before {\n    -webkit-user-select: none;\n    -webkit-mask-image: url(Images/statusbarButtonGlyphs.png);\n    -webkit-mask-size: 320px 144px;\n    content: \"a\";\n    color: transparent;\n    text-shadow: none;\n    margin-right: 1px;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.outline-disclosure li.parent::before {\n    -webkit-mask-image: url(Images/statusbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.outline-disclosure li.parent::before {\n    -webkit-mask-position: -4px -96px;\n    background-color: rgb(110, 110, 110);\n}\n\n.outline-disclosure li .selection {\n    display: none;\n    position: absolute;\n    left: 0;\n    right: 0;\n    height: 15px;\n    z-index: -1;\n}\n\n.outline-disclosure li.hovered:not(.selected) .selection {\n    display: block;\n    left: 3px;\n    right: 3px;\n    background-color: rgba(56, 121, 217, 0.1);\n    border-radius: 5px;\n}\n\n.outline-disclosure li.parent.expanded::before {\n    -webkit-mask-position: -20px -96px;\n}\n\n.outline-disclosure li.selected .selection {\n    display: block;\n    background-color: rgb(212, 212, 212);\n}\n\n.outline-disclosure ol {\n    list-style-type: none;\n    -webkit-padding-start: 12px;\n    margin: 0;\n}\n\n.outline-disclosure ol.children {\n    display: none;\n}\n\n.outline-disclosure ol.children.expanded {\n    display: block;\n}\n\n.outline-disclosure li .webkit-html-tag.close {\n    margin-left: -12px;\n}\n\n.outline-disclosure > ol {\n    position: relative;\n    margin: 0;\n    cursor: default;\n    min-width: 100%;\n    min-height: 100%;\n    -webkit-transform: translateZ(0);\n    padding-left: 2px;\n}\n\n.outline-disclosure ol:focus li.selected {\n    color: white;\n}\n\n.outline-disclosure ol:focus li.parent.selected::before {\n    background-color: white;\n}\n\n.outline-disclosure ol:focus li.selected * {\n    color: inherit;\n}\n\n.outline-disclosure ol:focus li.selected .selection {\n    background-color: rgb(56, 121, 217);\n}\n\n.elements-tree-outline li.shadow-root + ol {\n    margin-left: 5px;\n    padding-left: 5px;\n    border-left: 1px solid gray;\n}\n\n.elements-tree-editor {\n    -webkit-user-select: text;\n    -webkit-user-modify: read-write-plaintext-only;\n}\n\n.outline-disclosure li.elements-drag-over .selection {\n    display: block;\n    margin-top: -2px;\n    border-top: 2px solid rgb(56, 121, 217);\n}\n\n.outline-disclosure li.in-clipboard .highlight {\n    outline: 1px dotted darkgrey;\n}\n\n.CodeMirror {\n    /* Consistent with the .editing class in inspector.css */\n    box-shadow: rgba(0, 0, 0, .5) 3px 3px 4px;\n    outline: 1px solid rgb(66%, 66%, 66%) !important;\n    background-color: white;\n}\n\n.CodeMirror-lines {\n    padding: 0;\n}\n\n.CodeMirror pre {\n    padding: 0;\n}\n\nbutton, input, select {\n  font-family: inherit;\n  font-size: inherit;\n}\n\n.editing {\n    -webkit-user-select: text;\n    box-shadow: rgba(0, 0, 0, .5) 3px 3px 4px;\n    outline: 1px solid rgb(66%, 66%, 66%) !important;\n    background-color: white;\n    -webkit-user-modify: read-write-plaintext-only;\n    text-overflow: clip !important;\n    padding-left: 2px;\n    margin-left: -2px;\n    padding-right: 2px;\n    margin-right: -2px;\n    margin-bottom: -1px;\n    padding-bottom: 1px;\n    opacity: 1.0 !important;\n}\n\n.editing,\n.editing * {\n    color: #222 !important;\n    text-decoration: none !important;\n}\n\n.editing br {\n    display: none;\n}\n\n.elements-gutter-decoration {\n    position: absolute;\n    left: 1px;\n    margin-top: 2px;\n    height: 8px;\n    width: 8px;\n    border-radius: 4px;\n    border: 1px solid orange;\n    background-color: orange;\n}\n\n.elements-gutter-decoration.elements-has-decorated-children {\n    opacity: 0.5;\n}\n\n.add-attribute {\n    margin-left: 1px;\n    margin-right: 1px;\n    white-space: nowrap;\n}\n\n.elements-tree-element-pick-node-1 {\n    border-radius: 3px;\n    padding: 1px 0 1px 0;\n    -webkit-animation: elements-tree-element-pick-node-animation-1 0.5s 1;\n}\n\n.elements-tree-element-pick-node-2 {\n    border-radius: 3px;\n    padding: 1px 0 1px 0;\n    -webkit-animation: elements-tree-element-pick-node-animation-2 0.5s 1;\n}\n\n@-webkit-keyframes elements-tree-element-pick-node-animation-1 {\n    from { background-color: rgb(255, 210, 126); }\n    to { background-color: inherit; }\n}\n\n@-webkit-keyframes elements-tree-element-pick-node-animation-2 {\n    from { background-color: rgb(255, 210, 126); }\n    to { background-color: inherit; }\n}\n\n.pick-node-mode {\n    cursor: pointer;\n}\n\n.webkit-html-attribute-value a {\n    cursor: default !important;\n}\n\n.elements-tree-nowrap, .elements-tree-nowrap .li {\n    white-space: pre !important;\n}\n\n.outline-disclosure .elements-tree-nowrap li {\n    word-wrap: normal;\n}\n\n/* DOM update highlight */\n@-webkit-keyframes dom-update-highlight-animation {\n    from {\n        background-color: rgb(158, 54, 153);\n        color: white;\n    }\n    80% {\n        background-color: rgb(245, 219, 244);\n        color: inherit;\n    }\n    to {\n        background-color: inherit;\n    }\n}\n\n.dom-update-highlight {\n    -webkit-animation: dom-update-highlight-animation 1.4s 1 cubic-bezier(0, 0, 0.2, 1);\n    border-radius: 2px;\n}\n\n.outline-disclosure.single-node li {\n    padding-left: 2px;\n}\n\n\nli.parent.shadow-root {\n    /* display: none; */\n}\n\n.shadow-host-display-mode-toolbar .shadow-host-display-mode-toolbar-button {\n    border: 0;\n    margin: 0;\n    background-color: inherit;\n    height: 13px;\n    opacity: 0.2;\n}\n\n.shadow-host-display-mode-toolbar .shadow-host-display-mode-toolbar-button.toggled {\n    opacity: 0.7;\n}\n\n.shadow-host-display-mode-toolbar:hover .shadow-host-display-mode-toolbar-button {\n    opacity: 1;\n}\n\n.shadow-host-display-mode-toolbar-button.toggled {\n    color: rgb(66, 129, 235);\n}\n\n.shadow-host-display-mode-toolbar .shadow-host-display-mode-toolbar-button.toggled.custom-mode {\n    opacity: 1;\n}\n\n/*# sourceURL=elements/elementsTreeOutline.css */";Runtime.cachedResources["elements/spectrum.css"]="/* https://github.com/bgrins/spectrum */\n:host {\n    width: 205px;\n    height: 220px;\n    -webkit-user-select: none;\n}\n\n.spectrum-color {\n    position: absolute;\n    top: 5px;\n    left: 5px;\n    width: 158px;\n    height: 158px;\n    outline: 1px solid #bbb;\n}\n\n.spectrum-display-value {\n    -webkit-user-select: text;\n    display: inline-block;\n    padding-left: 2px;\n}\n\n.spectrum-hue {\n    position: absolute;\n    top: 5px;\n    right: 5px;\n    width: 28px;\n    height: 158px;\n    -webkit-box-reflect: right -28px;\n}\n\n.spectrum-range-container {\n    position: absolute;\n    bottom: 28px;\n    left: 5px;\n    display: flex;\n    align-items: center;\n}\n\n.spectrum-text {\n    position: absolute;\n    bottom: 5px;\n    left: 5px;\n    display: flex;\n    align-items: center;\n}\n\n.spectrum-range-container * {\n    font-size: 11px;\n    vertical-align: middle;\n}\n\n.spectrum-range-container label {\n    display: inline-block;\n    padding-right: 4px;\n}\n\n.spectrum-dragger,\n.spectrum-slider {\n    -webkit-user-select: none;\n}\n\n.spectrum-sat {\n    background-image: linear-gradient(to right, white, rgba(204, 154, 129, 0));\n}\n\n.spectrum-val {\n    background-image: linear-gradient(to top, black, rgba(204, 154, 129, 0));\n}\n\n.spectrum-hue {\n    background: linear-gradient(to top, #ff0000 0%, #ffff00 17%, #00ff00 33%, #00ffff 50%, #0000ff 67%, #ff00ff 83%, #ff0000 100%);\n}\n\n.spectrum-dragger {\n    border-radius: 5px;\n    height: 5px;\n    width: 5px;\n    border: 1px solid white;\n    cursor: pointer;\n    position: absolute;\n    top: 0;\n    left: 0;\n    background: black;\n}\n\n.spectrum-slider {\n    position: absolute;\n    top: 0;\n    cursor: pointer;\n    border: 1px solid black;\n    height: 4px;\n    left: -1px;\n    right: -1px;\n}\n\n.swatch {\n    width: 20px;\n    height:20px;\n    margin: 0;\n}\n\n.swatch-inner {\n    width: 100%;\n    height: 100%;\n    display: inline-block;\n    border: 1px solid rgba(128, 128, 128, 0.6);\n}\n\n.swatch-inner:hover {\n    border: 1px solid rgba(64, 64, 64, 0.8);\n}\n\n/*# sourceURL=elements/spectrum.css */";