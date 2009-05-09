
//OpenSCADA system module UI.WebVision file: VCA.js
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

MOD_ID = 'WebVision';
var isNN = navigator.appName.indexOf('Netscape') != -1;
var isIE = navigator.appName.indexOf('Microsoft') != -1;
var isOpera = navigator.appName.indexOf('Opera') != -1;
var isKonq = navigator.appName.indexOf('Konqueror') != -1;
/***************************************************
 * pathLev - Path parsing function.                *
 ***************************************************/
pathLev.off = 0;
function pathLev( path, level, scan )
{
  var an_dir = scan ? pathLev.off : 0;
  var t_lev = 0;
  var t_dir;
  while( an_dir<path.length && path.charAt(an_dir)=='/' ) an_dir++;
  if( an_dir >= path.length ) return '';
  while( true )
  {
    t_dir = path.indexOf('/',an_dir);
    if( t_dir < 0 ) { pathLev.off=path.length; return (t_lev==level)?path.substr(an_dir):''; }
    if( t_lev==level ) { pathLev.off=t_dir; return path.substr(an_dir,t_dir-an_dir); }
    an_dir = t_dir;
    t_lev++;
    while( an_dir<path.length && path.charAt(an_dir)=='/' ) an_dir++;
  }
}
/***************************************************
 * noSpace - Get no space string                   *
 ***************************************************/
function noSpace( str )
{
  var posSt = -1; var posEnd = 0;
  for( var i = 0; i < str.length; i++ )
    if( str[i] != ' ' ) { if(posSt<0) posSt=i; posEnd = i; }
  return (posSt>=0)?str.substr(posSt,posEnd-posSt+1):'';
}
/***************************************************
 * nodeText - Get DOM node text                    *
 ***************************************************/
function nodeText( node )
{
  var rez = '';
  for( var i = 0; i < node.childNodes.length; i++ )
    if( node.childNodes[i].nodeType == 3 ) rez += node.childNodes[i].data;
  return rez;
}
/***************************************************
 * setNodeText - Set DOM node text                 *
 ***************************************************/
function setNodeText( node, val )
{
  if( !node ) return;
  for( var i = 0; i < node.childNodes.length; i++ )
    if( node.childNodes[i].nodeType == 3 )
    { node.childNodes[i].data = val; return; }
  node.appendChild(node.ownerDocument.createTextNode(val));
}
/*****************************************************
 * nodeTextByTagId - Get DOM node by tag name and id *
 *****************************************************/
function nodeTextByTagId( node, tag, avl )
{
  for( var i = 0; i < node.childNodes.length; i++ )
    if( node.childNodes[i].nodeName == tag && node.childNodes[i].getAttribute('id') == avl )
      return nodeText(node.childNodes[i]);
  return '';
}
/***************************************************
 * posGetX - Get absolute position                 *
 **************************************************/
function posGetX(obj,noWScrl)
{
  var posX = 0;
  for( ; obj && obj.nodeName != 'BODY' ; obj=obj.parentNode )
    posX += (obj.style.left?parseInt(obj.style.left):0)+
	(obj.parentNode.style.borderLeftWidth?parseInt(obj.parentNode.style.borderLeftWidth):0)+
	(obj.parentNode.style.marginLeft?parseInt(obj.parentNode.style.marginLeft):0);
  return posX+(!noWScrl?-window.pageXOffset:0);
}
/***************************************************
 * posGetY - Get absolute position                 *
 **************************************************/
function posGetY(obj,noWScrl)
{
  var posY = 0;
  for( ; obj && obj.nodeName != 'BODY' ; obj=obj.parentNode )
    posY += (obj.style.top?parseInt(obj.style.top):0)+
	(obj.parentNode.style.borderTopWidth?parseInt(obj.parentNode.style.borderTopWidth):0)+
	(obj.parentNode.style.marginTop?parseInt(obj.parentNode.style.marginTop):0);
  return posY+(!noWScrl?-window.pageYOffset:0);
}
/***************************************************
 * getXmlHttp - Check and return XMLHttpRequest for*
 *  various browsers.                              *
 ***************************************************/
function getXmlHttp( )
{
  if( window.XMLHttpRequest ) return new XMLHttpRequest();
  else if( window.ActiveXObject )
  {
    try { return new ActiveXObject('Msxml2.XMLHTTP'); }
    catch(e) { return new ActiveXObject('Microsoft.XMLHTTP'); }
  }
  return null;
}
/***************************************************
 * realRound - Real numbers round                  *
 ***************************************************/
function realRound( val, dig, toInt )
{
  rez = Math.floor(val*Math.pow(10,dig?dig:0)+0.5)/Math.pow(10,dig?dig:0);
  if( toInt ) return Math.floor(rez+0.5);
  return rez;
}
/***************************************************
 * evMouseGet - Get mouse key code from event      *
 ***************************************************/
function evMouseGet( e )
{
    if(e.which == 1) return 'Left';
    else if(e.which == 2) return 'Midle';
    else if(e.which == 3) return 'Right';
}
/***************************************************
 * evKeyGet - Get key code from event              *
 ***************************************************/
function evKeyGet( e )
{
  if( (e.keyCode >= 48 && e.keyCode <= 57) || (e.keyCode >= 65 && e.keyCode <= 90) )
    return String.fromCharCode(e.keyCode);
  switch(e.keyCode)
  {
    case 27: return 'Esc';
    case 8:  return 'BackSpace';
    case 13: return 'Enter';
    case 45: return 'Insert';
    case 19: return 'Pause';
    case 36: return 'Home';
    case 35: return 'End';
    case 37: return 'Left';
    case 38: return 'Up';
    case 39: return 'Right';
    case 40: return 'Down';
    case 33: return 'PageUp';
    case 34: return 'PageDown';
    case 112: return 'F1';
    case 113: return 'F2';
    case 114: return 'F3';
    case 115: return 'F4';
    case 116: return 'F5';
    case 117: return 'F6';
    case 118: return 'F7';
    case 119: return 'F8';
    case 120: return 'F9';
    case 121: return 'F10';
    case 122: return 'F11';
    case 123: return 'F12';
    case 32: return 'Space';
    case 59: return 'Semicolon';
    case 61: return 'Equal';
  }
  if(isNN) switch(e.keyCode)
  {
    case 46: return 'Delete';
    case 44: return 'Print';
    case 192: return 'Apostrophe';
    case 106: return 'Asterisk';
    case 107: return 'Plus';
    case 188: return 'Comma';
    case 109: return 'Minus';
    case 190: return 'Period';
    case 220: return 'Slash';
    case 219: return 'BracketLeft';
    case 191: return 'BackSlash';
    case 221: return 'BracketRight';
    case 222: return 'QuoteLeft';
  }
  else switch(e.keyCode)
  {
    case 127: return 'Delete';
    case 96: return 'Apostrophe';
    case 42: return 'Asterisk';
    case 43: return 'Plus';
    case 44: return 'Comma';
    case 45: return 'Minus';
    case 46: return 'Period';
    case 92: return 'Slash';
    case 91: return 'BracketLeft';
    case 47: return 'BackSlash';
    case 93: return 'BracketRight';
    case 39: return 'QuoteLeft';
  }
  return '#'+e.keyCode.toString(16);
}
/***************************************************
 * servGet - XML get request to server             *
 ***************************************************/
function servGet( adr, prm )
{
  var req = getXmlHttp();
  req.open('GET',encodeURI('/'+MOD_ID+adr+'?'+prm),false);
  try
  { req.send(null);
    if( req.status == 200 && req.responseXML.childNodes.length )
      return req.responseXML.childNodes[0];
  } catch( e ) { window.location='/'+MOD_ID; }
}
/***************************************************
 * servSet - XML set request to server             *
 ***************************************************/
function servSet( adr, prm, body, waitRez )
{
  var req = getXmlHttp();
  req.open('POST',encodeURI('/'+MOD_ID+adr+'?'+prm),!waitRez);
  try
  { req.send(body);
    if( waitRez && req.status == 200 && req.responseXML.childNodes.length )
      return req.responseXML.childNodes[0];
  }
  catch( e ) { window.location='/'+MOD_ID; }
  return null;
}
/***************************************************
 * getWAttrs - request page/widget attributes      *
 ***************************************************/
/*function getWAttrs( wId, tm )
{
  var rNode = servGet(wId,'com=attrs'+(tm?('&tm='+tm):''))
  if( !rNode ) return;
  var atrLs = new Object();
  for( var i = 0; i < rNode.childNodes.length; i++ )
    if( rNode.childNodes[i].nodeType == 1 )
      atrLs[rNode.childNodes[i].getAttribute('id')] = nodeText(rNode.childNodes[i]);
  return atrLs;
}*/
/***************************************************
 * getWAttr - request page/widget attribute        *
 ***************************************************/
function getWAttr( wId, attr )
{
  var rNode = servGet(wId,'com=attr&attr='+attr);
  if( !rNode ) return null;
  return nodeText(rNode);
}
/***************************************************
 * setWAttrs - set page/widget attributes          *
 ***************************************************/
function setWAttrs( wId, attrs, val )
{
  var body = '<set>';
  if( typeof(attrs) == 'string' ) body+='<el id=\''+attrs+'\'>'+val+'</el>';
  else for( var i in attrs ) body+='<el id=\''+i+'\'>'+attrs[i]+'</el>';
  body+='</set>';
  servSet(wId,'com=attrs',body);
}
/***************************************************
 * setFocus - Command for set focus                *
 ***************************************************/
function setFocus( wdg, onlyClr )
{
  if( masterPage.focusWdf && masterPage.focusWdf == wdg ) return;
  var attrs = new Object();
  if( masterPage.focusWdf ) { attrs.focus = '0'; attrs.event = 'ws_FocusOut'; setWAttrs(masterPage.focusWdf,attrs); }
  masterPage.focusWdf = wdg;
  if( onlyClr ) return;
  attrs.focus = '1'; attrs.event = 'ws_FocusIn'; setWAttrs(masterPage.focusWdf,attrs);
}
/***************************************************
 * pwDescr - Page/widget descriptor object         *
 ***************************************************/
function callPage( pgId, updWdg, pgGrp, pgOpenSrc )
{
  if( !pgId ) return true;
  //> Check and update present page
  if( this == masterPage )
  {
    var opPg = this.findOpenPage(pgId);
    if( opPg && updWdg ) opPg.makeEl(servGet(pgId,'com=attrsBr&tm='+tmCnt));
    if( opPg ) return true;
  }
  //> Create or replace main page
  if( !pgGrp ) pgGrp = getWAttr(pgId,'pgGrp');
  if( !pgOpenSrc ) pgOpenSrc = getWAttr(pgId,'pgOpenSrc');
  if( !this.addr.length || (this == masterPage && pgGrp == 'main') || pgGrp == this.attrs['pgGrp'] )
  {
    this.addr  = pgId;
    this.place = document.createElement('div');
    this.makeEl(servGet(pgId,'com=attrsBr'));
    document.body.appendChild(this.place);
    //> Set project's icon and RunTime page title
    document.getElementsByTagName('link')[0].setAttribute('href',location.pathname+'?com=ico');
    return true;
  }
  //> Find for include page creation
  for( var i in this.wdgs )
    if( this.wdgs[i].attrs['root'] == 'Box' )
    {
      if( pgGrp == this.wdgs[i].attrs['pgGrp'] && pgId != this.wdgs[i].attrs['pgOpenSrc'] )
      {
	this.wdgs[i].attrs['pgOpenSrc'] = pgId;
	this.wdgs[i].makeEl(null,true);
	setWAttrs(this.wdgs[i].addr,'pgOpenSrc',pgId);
	return true;
      }
      if( this.wdgs[i].inclOpen && this.wdgs[i].pages[this.wdgs[i].inclOpen].callPage(pgId,updWdg,pgGrp,pgOpenSrc) ) return true;
    }
  //> Put checking to child pages
  for( var i in this.pages )
    if( this.pages[i].callPage(pgId,updWdg,pgGrp,pgOpenSrc) ) return true;
  //> Check for open child page or for unknown and empty source pages open as master page child windows
  if( pgOpenSrc == this.addr || this == masterPage )
  {
    var iPg = new pwDescr(pgId,true,this);
    iPg.window = window.open('',pgId,'width=600,height=400,directories=no,menubar=no,toolbar=no,scrollbars=yes,dependent=yes,location=no,locationbar=no,status=no,statusbar=no,alwaysRaised=yes');
    if( !iPg.window ) return true;
    iPg.window.document.open( );
    iPg.window.document.write("<html><body style='background-color: #E6E6E6;'><div id='main'/></body></html>\n");
    iPg.window.document.close( );
    var mainDiv = iPg.window.document.getElementById('main');
    iPg.place = mainDiv;
    this.pages[pgId] = iPg;
    iPg.makeEl( servGet(pgId,'com=attrsBr') );
    return true;
  }

  return false;
}
function findOpenPage( pgId )
{
  var opPg;
  if( pgId == this.addr ) return this;
  //> Check from included widgets
  for( var i in this.wdgs )
    if( this.wdgs[i].attrs['root'] == 'Box' )
    {
      if( pgId == this.wdgs[i].attrs['pgOpenSrc'] ) return this.wdgs[i].pages[pgId];
      if( this.wdgs[i].inclOpen )
      {
	opPg = this.wdgs[i].pages[this.wdgs[i].inclOpen].findOpenPage(pgId);
	if( opPg ) return opPg;
      }
    }
  //> Put checking to child pages
  for( var i in this.pages )
  {
    opPg = this.pages[i].findOpenPage(pgId);
    if( opPg ) return opPg;
  }
  return null;
}
function makeEl( pgBr, inclPg )
{
  var margBrdUpd = false; var newAttr = false;
  this.place.wdgLnk = this;
  if( !inclPg && pgBr )
    for( var j = 0; j < pgBr.childNodes.length; j++ )
    {
      if( pgBr.childNodes[j].nodeName != 'el' ) continue;
      var i = pgBr.childNodes[j].getAttribute('id');
      if( (i == 'bordWidth' || i == 'geomMargin') && this.attrs[i] != nodeText(pgBr.childNodes[j]) ) margBrdUpd = true;
      this.attrs[i] = nodeText(pgBr.childNodes[j]);
      newAttr = true;
    }
  if( newAttr || inclPg || !pgBr )
  {
  var elMargin = parseInt(this.attrs['geomMargin']);
  var elBorder = 0;
  if( this.attrs['bordWidth'] ) elBorder=parseInt(this.attrs['bordWidth']);
  var elStyle = '';
  if( !parseInt(this.attrs['en']) ) elStyle+='visibility : hidden; ';
  var geomX = parseFloat(this.attrs['geomX']);
  var geomY = parseFloat(this.attrs['geomY']);
  if( this.pg ) geomX = geomY = 0;
  else{ geomX *= this.parent.xScale(true); geomY *= this.parent.yScale(true); }
  if( this.parent && !(this.pg && this.parent.pg) )
  {
    geomX -= parseInt(this.parent.attrs['geomMargin'])+parseInt(this.parent.attrs['bordWidth']);
    geomY -= parseInt(this.parent.attrs['geomMargin'])+parseInt(this.parent.attrs['bordWidth']);
  }
  elStyle+='position: absolute; left: '+realRound(geomX)+'px; top: '+realRound(geomY)+'px; ';
  var geomW = parseFloat(this.attrs['geomW'])-2*(elMargin+elBorder);
  var geomH = parseFloat(this.attrs['geomH'])-2*(elMargin+elBorder);

  if( this.pg && this.parent && this.parent.inclOpen && this.parent.inclOpen == this.addr )
  {
    elStyle += 'overflow: auto; ';
    geomW = parseFloat(this.parent.attrs['geomW'])-2*(elMargin+elBorder);
    geomH = parseFloat(this.parent.attrs['geomH'])-2*(elMargin+elBorder);
  }
  else elStyle += 'overflow: hidden; ';

  var xSc = this.xScale(true);
  var ySc = this.yScale(true);
  geomW = realRound(geomW*xSc); geomH = realRound(geomH*ySc);

  //> Set included window geometry to widget size
//  if( this == masterPage ) resizeTo(geomW,geomH);
  if( this.pg && this.window )
  {
    var pgWin = this.window;
    pgWin.document.body.style.backgroundColor = this.attrs['backColor'];
    pgWin.resizeTo(geomW+20,geomH+40);
  }

  if( parseInt(this.attrs['focus']) ) setFocus(this.addr,true);

  if( !(parseInt(this.attrs['perm'])&SEQ_RD) )
  {
    if( this.pg )
    {
      elStyle+='background-color: #818181; border: 1px solid black; color: red; overflow: auto; ';
      this.place.innerHTML = "<div class='vertalign' style='width: "+(geomW-2)+"px; height: "+(geomH-2)+"px;'>Page: '"+this.addr+"'.<br/>View access is no permited.</div>";
    }
  }
  else if( this.attrs['root'] == 'ElFigure' )
  {
    if( this.attrs['backColor'] ) elStyle+='background-color: '+this.attrs['backColor']+'; ';
    if( this.attrs['backImg'] )
      elStyle+='background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['backImg']+'\'); ';
    var figObj = this.place.childNodes[0];
    if( !figObj )
    {
      figObj = this.place.ownerDocument.createElement('img');
      figObj.width = geomW; figObj.height = geomH;
      figObj.border = 0;
      figObj.wdgLnk = this;
      this.place.appendChild(figObj);
    }
    figObj.src = '/'+MOD_ID+this.addr+'?com=obj&tm='+tmCnt+'&xSc='+xSc.toFixed(2)+'&ySc='+ySc.toFixed(2);
    if( parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR )
    {
      figObj.onclick = function(e)
      {
	if(!e) e = window.event;
	servSet(this.wdgLnk.addr,'com=obj&sub=point&xSc='+xSc.toFixed(2)+'&ySc='+ySc.toFixed(2)+
						  '&x='+(e.offsetX?e.offsetX:(e.clientX-posGetX(this)))+
						  '&y='+(e.offsetY?e.offsetY:(e.clientY-posGetY(this)))+
						  '&key='+evMouseGet(e),'');
	return false;
      }
      figObj.ondblclick = function(e)
      {
	if(!e) e = window.event;
	servSet(this.wdgLnk.addr,'com=obj&sub=point&xSc='+xSc.toFixed(2)+'&ySc='+ySc.toFixed(2)+
						  '&x='+(e.offsetX?e.offsetX:(e.clientX-posGetX(this)))+
						  '&y='+(e.offsetY?e.offsetY:(e.clientY-posGetY(this)))+
						  '&key=DblClick','');
      }
    }
    else { figObj.onclick = ''; figObj.ondblclick = ''; }
  }
  else if( this.attrs['root'] == 'Box' )
  {
    if( this.attrs['backColor'] ) elStyle+='background-color: '+this.attrs['backColor']+'; ';
    if( this.attrs['backImg'] )   elStyle+='background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['backImg']+'\'); ';
    if( parseInt(this.attrs['focus']) ) elStyle+='border-style: ridge; border-width: 2px; border-color: white; ';
    else {
      elStyle+='border-style: solid; border-width: '+this.attrs['bordWidth']+'px; ';
      if( this.attrs['bordColor'] ) elStyle+='border-color: '+this.attrs['bordColor']+'; ';
    }
    switch( parseInt(this.attrs['bordStyle']) )
    {
      case 1: elStyle+='border-style: dotted; '; break;
      case 2: elStyle+='border-style: dashed; '; break;
      case 3: elStyle+='border-style: solid; ';  break;
      case 4: elStyle+='border-style: double; '; break;
      case 5: elStyle+='border-style: groove; '; break;
      case 6: elStyle+='border-style: ridge; ';  break;
      case 7: elStyle+='border-style: inset; ';  break;
      case 8: elStyle+='border-style: outset; '; break;
    }
    if( !this.pg && ((this.inclOpen && this.attrs['pgOpenSrc'] != this.inclOpen) || (!this.inclOpen && this.attrs['pgOpenSrc'].length)) )
    {
      if( this.inclOpen )
      {
	servSet(this.inclOpen,'com=pgClose','');
	pgCache[this.inclOpen] = this.pages[this.inclOpen];
	pgCache[this.inclOpen].reqTm = tmCnt;
	this.place.removeChild(this.pages[this.inclOpen].place);
	this.pages[this.inclOpen].perUpdtEn(false);
	delete this.pages[this.inclOpen];
	this.inclOpen = null;
      }
      if( this.attrs['pgOpenSrc'].length )
      {
        this.inclOpen = this.attrs['pgOpenSrc'];
        if( pgCache[this.inclOpen] )
        {
	  this.pages[this.inclOpen] = pgCache[this.inclOpen];
	  this.place.appendChild(this.pages[this.inclOpen].place);
	  pgBr = servGet(this.inclOpen,'com=attrsBr&tm='+pgCache[this.inclOpen].reqTm);
	  this.pages[this.inclOpen].perUpdtEn(true);
	  this.pages[this.inclOpen].makeEl(pgBr);
	  delete pgCache[this.inclOpen];
        }
        else
        {
	  var iPg = new pwDescr(this.inclOpen,true,this);
	  iPg.place = this.place.ownerDocument.createElement('div');
	  iPg.makeEl( servGet(this.inclOpen,'com=attrsBr') );
	  this.pages[this.inclOpen] = iPg;
	  this.place.appendChild(iPg.place);
        }
      }
    }
    this.place.wdgLnk = this;
    if( parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR )
      this.place.onclick = function() { setFocus(this.wdgLnk.addr); return false; };
    else this.place.onclick = '';
  }
  else if( this.attrs['root'] == 'Text' )
  {
    if( this.attrs['backColor'] ) elStyle+='background-color: '+this.attrs['backColor']+'; ';
    if( this.attrs['backImg'] )   elStyle+='background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['backImg']+'\'); ';
    elStyle+='border-style: solid; border-width: '+this.attrs['bordWidth']+'px; ';
    if( this.attrs['bordColor'] ) elStyle+='border-color: '+this.attrs['bordColor']+'; ';
    var txtAlign = parseInt(this.attrs['alignment']);
    var spanStyle = 'display: table-cell; width: '+geomW+'px; height: '+geomH+'px; ';
    switch(txtAlign&0x3)
    {
      case 0: spanStyle+='text-align: left; '; break;
      case 1: spanStyle+='text-align: right; '; break;
      case 2: spanStyle+='text-align: center; '; break;
      case 3: spanStyle+='text-align: justify; '; break;
    }
    switch(txtAlign>>2)
    {
      case 0: spanStyle+='vertical-align: top; '; break;
      case 1: spanStyle+='vertical-align: bottom; '; break;
      case 2: spanStyle+='vertical-align: middle; '; break;
    }
    var allFnt = this.attrs['font'].split(' ');
    if( allFnt.length >= 1 ) spanStyle+='font-family: '+allFnt[0].replace(/_/g,' ')+'; ';
    if( allFnt.length >= 2 ) spanStyle+='font-size: '+(parseInt(allFnt[1])*Math.min(xSc,ySc)).toFixed(0)+'px; ';
    if( allFnt.length >= 3 ) spanStyle+='font-weight: '+(parseInt(allFnt[2])?'bold':'normal')+'; ';
    if( allFnt.length >= 4 ) spanStyle+='font-style: '+(parseInt(allFnt[3])?'italic':'normal')+'; ';
    spanStyle+='color: '+(this.attrs['color']?this.attrs['color']:'black')+'; ';
    var txtVal = this.attrs['text'];
    for( var i = 0; i < parseInt(this.attrs['numbArg']); i++ )
    {
      var argVal;
      var argCfg = new Array();
      switch(parseInt(this.attrs['arg'+i+'tp']))
      {
	case 0: case 2:
	  argCfg[0]=this.attrs['arg'+i+'cfg'];
	  argVal=this.attrs['arg'+i+'val'];
	  break;
	case 1:
	  argCfg=this.attrs['arg'+i+'cfg'].split(';');
	  if( argCfg[1] == 'g' )      argVal=parseFloat(this.attrs['arg'+i+'val']).toPrecision(argCfg[2]);
	  else if( argCfg[1] == 'f' ) argVal=parseFloat(this.attrs['arg'+i+'val']).toFixed(argCfg[2]);
	  else argVal=this.attrs['arg'+i+'val'];
	  break;
      }
      var argSize = parseInt(argCfg[0]);
      var argPad = '';
      for( var j = argVal.length; j < Math.abs(argSize); j++ ) argPad+='&nbsp;';
      if( argSize > 0 ) argVal=argPad+argVal; else argVal+=argPad;
      txtVal = txtVal.replace('%'+(i+1),argVal);
    }
    var txtVal1 = '';
    for( var j = 0; j < txtVal.length; j++ )
      if( txtVal[j] == '\n' ) txtVal1+='<br />'; else txtVal1+=txtVal[j];
//    txtVal.replace(/\n/g,'<br />');
//    while(this.place.childNodes.length) this.place.removeChild(this.place.childNodes[0]);
    this.place.innerHTML = "<span style='"+spanStyle+"'>"+txtVal1+"</span>";
    this.place.wdgLnk = this;
    if( parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR )
      this.place.onclick = function() { setFocus(this.wdgLnk.addr); return false; };
    else this.place.onclick = '';
  }
  else if( this.attrs['root'] == 'Media' )
  {
    if( this.attrs['backColor'] ) elStyle+='background-color: '+this.attrs['backColor']+'; ';
    if( this.attrs['backImg'] )
      elStyle+='background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['backImg']+'\'); ';
    elStyle+='border-style: solid; border-width: '+this.attrs['bordWidth']+'px; ';
    if( this.attrs['bordColor'] ) elStyle+='border-color: '+this.attrs['bordColor']+'; ';
    while(this.place.childNodes.length) this.place.removeChild(this.place.childNodes[0]);
    var medObj = this.place.ownerDocument.createElement('img');
    medObj.src = this.attrs['src'].length ? ('/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['src']) : '';
    medObj.border = 0;
    if( this.attrs['fit'] == 1 ) { medObj.width = geomW; medObj.height = geomH; }
    if( parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR )
    {
      if( parseInt(this.attrs['areas']) )
      {
	var mapObj = this.place.ownerDocument.createElement('map');
	mapObj.name = this.addr;
	for( var i = 0; i <  parseInt(this.attrs['areas']); i++ )
	{
	  var arObj = this.place.ownerDocument.createElement('area');
	  switch(parseInt(this.attrs['area'+i+'shp']))
	  {
	    case 0: arObj.shape = 'rect'; break;
	    case 1: arObj.shape = 'poly'; break;
	    case 2: arObj.shape = 'circle'; break;
	  }
	  arObj.coords = this.attrs['area'+i+'coord'];
	  arObj.title = this.attrs['area'+i+'title'];
	  arObj.href = '';
	  arObj.onclick = function() { setWAttrs(this.wdgLnk.addr,'event','ws_MapAct'+i+'Left'); return false; }
	  arObj.wdgLnk = this;
	  mapObj.appendChild(arObj);
	}
	this.place.appendChild(mapObj);
	medObj.setAttribute('usemap','#'+this.addr);
	this.place.appendChild(medObj);
      }
    }
    else this.place.appendChild(medObj);
  }
  else if( this.attrs['root'] == 'FormEl' && !this.place.isModify )
  {
    var elTp = parseInt(this.attrs['elType']);
    while( this.place.childNodes.length ) this.place.removeChild(this.place.childNodes[0]);
    var fontCfg = '';
    if( this.attrs['font'] )
    {
      var allFnt = this.attrs['font'].split(' ');
      if( allFnt.length >= 3 && parseInt(allFnt[2]) ) fontCfg += 'bold ';
      if( allFnt.length >= 4 && parseInt(allFnt[3]) ) fontCfg += 'italic ';
      if( allFnt.length >= 2 ) fontCfg += (parseInt(allFnt[1])*Math.min(xSc,ySc)).toFixed(0)+'px ';
      if( allFnt.length >= 1 ) fontCfg += allFnt[0].replace(/_/g,' ')+' ';
    }
    switch(elTp)
    {
      case 0:	//Line edit
	var formObj = this.place.ownerDocument.createElement('input');
	formObj.disabled = !( parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR );
	formObj.tabIndex = parseInt(this.attrs['geomZ'])+1;
	this.place.appendChild(formObj);
	var geomWint = geomW-4;
	formObj.wdgLnk = this;
	formObj.style.cssText = 'position: absolute; top: '+((geomH-20)/2)+'px; width: '+geomWint+'px; height: '+Math.min(geomH,16)+'px; border: 1px solid black; font: '+fontCfg+'; padding: 1px;';
	this.place.view = parseInt(this.attrs['view']);
	this.place.cfg = this.attrs['cfg'];
	if( parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR )
	{
	 switch(this.place.view)
	 {
	  case 1:		//Combo
	    var combImg = this.place.ownerDocument.createElement('img');
	    combImg.src = '/'+MOD_ID+'/img_combar';
	    combImg.style.cssText = 'position: absolute; left: '+(geomW-16)+'px; top: '+((geomH-20)/2)+'px; width: 16px; height: 20px; cursor: pointer; ';
	    this.place.appendChild(combImg);
	    formObj.style.width = (geomWint-16)+'px';
	    combImg.onclick = function()
	    {
	      var formObj = this.parentNode.childNodes[0];
	      var combList = this.ownerDocument.getElementById('#combo');
	      if( !combList )
	      {
		combList = this.ownerDocument.createElement('div');
		combList.id = '#combo';
		combList.appendChild(this.ownerDocument.createElement('select'));
		combList.childNodes[0].size='100';
		combList.childNodes[0].onchange = function() { this.formObj.value = this.options[this.selectedIndex].value; this.formObj.setModify(true); this.parentNode.style.visibility = 'hidden'; }
		combList.childNodes[0].onblur = function() { this.parentNode.style.visibility = 'hidden'; this.parentNode.style.top = "-100px"; }
		this.ownerDocument.body.appendChild(combList);
	      }
	      while(combList.childNodes[0].childNodes.length) combList.childNodes[0].removeChild(combList.childNodes[0].childNodes[0]);
	      var elLst = formObj.parentNode.cfg.split('\n');
	      for( var i = 0; i < elLst.length; i++ )
	      {
		var optEl = this.ownerDocument.createElement('option');
		optEl.appendChild(this.ownerDocument.createTextNode(elLst[i]));
		if( formObj.valGet() == elLst[i] ) optEl.defaultSelected=optEl.selected=true;
		combList.childNodes[0].appendChild(optEl);
	      }
	      if( combList.childNodes[0].childNodes.length )
	      {
		combList.style.cssText = 'position: absolute; visibility : visible; left: '+posGetX(formObj,true)+'px; top: '+(posGetY(formObj,true)+formObj.offsetHeight)+'px; width: '+formObj.offsetWidth+'px; height: '+Math.min(elLst.length*15,70)+'px; border: 0; ';
		combList.childNodes[0].style.cssText = 'width: '+formObj.offsetWidth+'px; height: '+Math.min(elLst.length*15,70)+'px; border-width: 2px; font: '+fontCfg+'; padding: 1px;';
		combList.childNodes[0].formObj = formObj;
		combList.childNodes[0].focus();
	      }
	      return false;
	    }
	    break;
	  case 2: case 3:	//Integer, Real
	    var spinImg = this.place.ownerDocument.createElement('img');
	    spinImg.src = '/'+MOD_ID+'/img_spinar';
	    spinImg.style.cssText = 'position: absolute; left: '+(geomW-16)+'px; top: '+((geomH-20)/2)+'px; width: 16px; height: 20px; cursor: pointer; ';
	    spinImg.border = '0';
	    formObj.style.width = (geomWint-16)+'px';
	    spinImg.onclick = function(e)
	    {
	      if( !e ) e = window.event;
	      var formObj = this.parentNode.childNodes[0];
	      var argCfg = this.parentNode.cfg.split(':');
	      if( (e.clientY-posGetY(this)) < 10 )
		formObj.valSet(formObj.valGet()+((argCfg.length>2)?((this.parentNode.view==2)?parseInt(argCfg[2]):parseFloat(argCfg[2])):1));
	      else formObj.valSet(formObj.valGet()-((argCfg.length>2)?((this.parentNode.view==2)?parseInt(argCfg[2]):parseFloat(argCfg[2])):1));
	      formObj.setModify(true);
	      return false;
	    }
	    this.place.appendChild(spinImg);
	    break;
	 }
	 formObj.onkeyup = function(e)
	 {
	  if( !e ) e = window.event;
	  if( this.modify() && e.keyCode == 13 ) this.chApply();
	  if( this.modify() && e.keyCode == 27 ) this.chEscape();
	  if( this.saveVal != this.value ) this.setModify(true);
	 }
	 formObj.modify = function( )
	 { return (this.parentNode.childNodes[(this.parentNode.view>=1&&this.parentNode.view<=3)?2:1].style.visibility == 'visible'); }
	 formObj.setModify = function( on )
	 {
	  if( this.modify() == on ) return;
	  var posOkImg = (this.parentNode.view>=1&&this.parentNode.view<=3)?2:1;
	  var okImg = this.parentNode.childNodes[posOkImg];
	  if( on ) { this.style.width = (parseInt(this.style.width)-16)+'px'; if(posOkImg==2) this.parentNode.childNodes[1].style.left = (parseInt(this.parentNode.childNodes[1].style.left)-16)+'px'; okImg.style.visibility = 'visible'; }
	  else     { this.style.width = (parseInt(this.style.width)+16)+'px'; if(posOkImg==2) this.parentNode.childNodes[1].style.left = (parseInt(this.parentNode.childNodes[1].style.left)+16)+'px'; okImg.style.visibility = 'hidden'; }
	  this.parentNode.isModify = on;
	 }
	}
	formObj.valSet = function(val)
	{
	  switch(this.parentNode.view)
	  {
	    case 0: case 1: this.value = val; break;
	    case 2:
	      var argCfg = this.parentNode.cfg.split(':');
	      this.value = ((argCfg.length>3)?argCfg[3]:'')+Math.max((argCfg.length>1)?parseInt(argCfg[0]):0,Math.min((argCfg.length>1)?parseInt(argCfg[1]):100,parseInt(val)))+((argCfg.length>4)?argCfg[4]:'');
	      break;
	    case 3:
	      var argCfg = this.parentNode.cfg.split(':');
	      this.value = ((argCfg.length>3)?argCfg[3]:'')+Math.max((argCfg.length>1)?parseFloat(argCfg[0]):0,Math.min((argCfg.length>1)?parseFloat(argCfg[1]):100,parseFloat(val))).toFixed((argCfg.length>5)?parseInt(argCfg[5]):2)+((argCfg.length>4)?argCfg[4]:'');
	      break;
	    case 4:
	      var rez = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'hh:mm';
	      var v = (Math.floor(val/3600)%(ap?12:24)).toString(10); rez = rez.replace('hh',(v.length==1)?'0'+v:v); rez = rez.replace('h',v);
	      v = (Math.floor(val/60)%60).toString(10); rez = rez.replace('mm',(v.length==1)?'0'+v:v); rez = rez.replace('m',v);
	      v = (val%60).toString(10); rez = rez.replace('ss',(v.length==1)?'0'+v:v); rez = rez.replace('s',v);
	      if( rez.indexOf('ap') >= 0 ) { rez = rez.replace('ap',(val>=43200)?'pm':'am'); var ap = true; }
	      if( rez.indexOf('AP') >= 0 ) { rez = rez.replace('AP',(val>=43200)?'PM':'AM'); var ap = true; }
	      this.value = rez;
	      break;
	    case 5:
	      var rez = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'dd.MM.yy';
	      var dt = new Date(parseInt(val)*1000);
	      var v = dt.getDate().toString(10); rez = rez.replace('dddd',(v.length==1)?'0'+v:v); rez = rez.replace('ddd',(v.length==1)?'0'+v:v); rez = rez.replace('dd',(v.length==1)?'0'+v:v); rez = rez.replace('d',v);
	      v = (dt.getMonth()+1).toString(10); rez = rez.replace('MMMM',(v.length==1)?'0'+v:v); rez = rez.replace('MMM',(v.length==1)?'0'+v:v); rez = rez.replace('MM',(v.length==1)?'0'+v:v); rez = rez.replace('M',v);
	      v = dt.getFullYear().toString(10); rez = rez.replace('yyyy',v); rez = rez.replace('yy',v.substr(2,2));
	      delete dt;
	      this.value = rez;
	      break;
	    case 6:
	      var rez = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'dd.MM.yy hh:mm';
	      var dt = new Date(parseInt(val)*1000);
	      var v = dt.getDate().toString(10); rez = rez.replace('dddd',(v.length==1)?'0'+v:v); rez = rez.replace('ddd',(v.length==1)?'0'+v:v); rez = rez.replace('dd',(v.length==1)?'0'+v:v); rez = rez.replace('d',v);
	      v = (dt.getMonth()+1).toString(10); rez = rez.replace('MMMM',(v.length==1)?'0'+v:v); rez = rez.replace('MMM',(v.length==1)?'0'+v:v); rez = rez.replace('MM',(v.length==1)?'0'+v:v); rez = rez.replace('M',v);
	      v = dt.getFullYear().toString(10); rez = rez.replace('yyyy',v); rez = rez.replace('yy',v.substr(2,2));
	      v = dt.getHours().toString(10); rez = rez.replace('hh',(v.length==1)?'0'+v:v); rez = rez.replace('h',v);
	      v = dt.getMinutes().toString(10); rez = rez.replace('mm',(v.length==1)?'0'+v:v); rez = rez.replace('m',v);
	      v = dt.getSeconds().toString(10); rez = rez.replace('ss',(v.length==1)?'0'+v:v); rez = rez.replace('s',v);
	      if( rez.indexOf('ap') >= 0 ) { rez = rez.replace('ap',(val>=43200)?'pm':'am'); var ap = true; }
	      if( rez.indexOf('AP') >= 0 ) { rez = rez.replace('AP',(val>=43200)?'PM':'AM'); var ap = true; }
	      delete dt;
	      this.value = rez;
	      break;
	  }
	  this.saveVal = this.value;
	  this.srcVal = val;
	}
	formObj.valGet = function( )
	{
	  switch(this.parentNode.view)
	  {
	    case 0: case 1: return this.value;
	    case 2:
	      var rez = this.value;
	      var argCfg = this.parentNode.cfg.split(':');
	      var tmp = (argCfg&&argCfg.length>3)?noSpace(argCfg[3]):'';
	      if( tmp.length && rez.indexOf(tmp) >= 0 ) rez=rez.substring(rez.indexOf(tmp)+tmp.length);
	      tmp = (argCfg&&argCfg.length>4)?noSpace(argCfg[4]):'';
	      if( tmp.length && rez.indexOf(tmp) >= 0 ) rez=rez.substring(0,rez.indexOf(tmp));
	      return parseInt(rez);
	    case 3:
	      var rez = this.value;
	      var argCfg = this.parentNode.cfg.split(':');
	      var tmp = (argCfg&&argCfg.length>3)?noSpace(argCfg[3]):'';
	      if( tmp.length && rez.indexOf(tmp) >= 0 ) rez=rez.substring(rez.indexOf(tmp)+tmp.length);
	      tmp = (argCfg&&argCfg.length>4)?noSpace(argCfg[4]):'';
	      if( tmp.length && rez.indexOf(tmp) >= 0 ) rez=rez.substring(0,rez.indexOf(tmp));
	      return parseFloat(rez);
	    case 4:
	      var cfg  = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'hh:mm';
	      var rez  = this.value;
	      if( cfg.indexOf('ap') >= 0 || cfg.indexOf('AP') >= 0 )
	      { cfg = cfg.replace('ap',''); cfg = cfg.replace('AP',''); rez = rez.replace('am',''); rez = rez.replace('AM',''); rez = rez.replace('pm',''); rez = rez.replace('PM',''); }
	      var hour = Math.floor(this.srcVal/3600);
	      var min  = Math.floor(this.srcVal/60)%60;
	      var sec  = this.srcVal%60;
	      var i = 0; var lstS = ''; var lenS = 0;
	      while( true )
	      {
		if( i >= cfg.length || cfg.charAt(i) == 'h' || cfg.charAt(i) == 'm' || cfg.charAt(i) == 's' )
		{
		  var space = lenS ? noSpace(cfg.substr(i-lenS,lenS)) : ''; if( lenS && !space.length ) space = ' ';
		  if( (space.length && lstS != '') || i >= cfg.length )
		  {
		    if( space.length && rez.indexOf(space) == -1 ) return 0;
		    var val = parseInt(space.length?rez.substr(0,rez.indexOf(space)):rez,10);
		    rez = rez.substr(rez.indexOf(space));
		    if( lstS == 'h' ) hour = val; else if( lstS == 'm' ) min = val; else if( lstS == 's' ) sec = val;
		    if( i >= cfg.length ) break;
		  }
		  if( space.length )
		  {
		    if( rez.indexOf(space) == -1 ) return 0;
		    rez = rez.substr(rez.indexOf(space)+space.length);
		  }
		  lstS = cfg.charAt(i); lenS = 0; i++; continue;
		}
		lenS++; i++;
	      }
	      if( this.value.indexOf('pm') >= 0 || this.value.indexOf('PM') >= 0 ) hour+=12;
	      return (hour%24)*3600+(min%60)*60+(sec%60);
	    case 5:
	      var cfg  = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'd.MM.yy';
	      var rez  = this.value;
	      var dt = new Date(this.srcVal*1000);
	      var i = 0; var lstS = ''; var lenS = 0;
	      while( true )
	      {
		if( i >= cfg.length || cfg.charAt(i) == 'd' || cfg.charAt(i) == 'M' || cfg.charAt(i) == 'y' )
		{
		  var space = lenS ? noSpace(cfg.substr(i-lenS,lenS)) : ''; if( lenS && !space.length ) space = ' ';
		  if( (space.length && lstS != '') || i >= cfg.length )
		  {
		    if( space.length && rez.indexOf(space) == -1 ) return 0;
		    var val = parseInt(space.length?rez.substr(0,rez.indexOf(space)):rez,10);
		    rez = rez.substr(rez.indexOf(space));
		    if( lstS == 'd' ) dt.setDate(val); else if( lstS == 'M' ) dt.setMonth(val-1); else if( lstS == 'y' ) dt.setFullYear((val<100)?2000+val:val);
		    if( i >= cfg.length ) break;
		  }
		  if( space.length )
		  {
		    if( rez.indexOf(space) == -1 ) return 0;
		    rez = rez.substr(rez.indexOf(space)+space.length);
		  }
		  lstS = cfg.charAt(i); lenS = 0; i++; continue;
		}
		lenS++; i++;
	      }
	      return dt.getTime()/1000;
	    case 6:
	      var cfg = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'dd.MM.yy hh:mm';
	      var rez  = this.value;
	      if( cfg.indexOf('ap') >= 0 || cfg.indexOf('AP') >= 0 )
	      { cfg = cfg.replace('ap',''); cfg = cfg.replace('AP',''); rez = rez.replace('am',''); rez = rez.replace('AM',''); rez = rez.replace('pm',''); rez = rez.replace('PM',''); }
	      var dt = new Date(this.srcVal*1000);
	      var i = 0; var lstS = ''; var lenS = 0;
	      while( true )
	      {
		if( i >= cfg.length || cfg.charAt(i) == 'd' || cfg.charAt(i) == 'M' || cfg.charAt(i) == 'y' || cfg.charAt(i) == 'h' || cfg.charAt(i) == 'm' || cfg.charAt(i) == 's' )
		{
		  var space = lenS ? noSpace(cfg.substr(i-lenS,lenS)) : ''; if( lenS && !space.length ) space = ' ';
		  if( (space.length && lstS != '') || i >= cfg.length )
		  {
		    if( space.length && rez.indexOf(space) == -1 ) return 0;
		    var val = parseInt(space.length?rez.substr(0,rez.indexOf(space)):rez,10);
		    rez = rez.substr(rez.indexOf(space));
		    if( lstS == 'd' ) dt.setDate(val); else if( lstS == 'M' ) dt.setMonth(val-1); else if( lstS == 'y' ) dt.setFullYear((val<100)?2000+val:val);
		    else if( lstS == 'h' ) dt.setHours(val); else if( lstS == 'm' ) dt.setMinutes(val); else if( lstS == 's' ) dt.setSeconds(val);
		    if( i >= cfg.length ) break;
		  }
		  if( space.length )
		  {
		    if( rez.indexOf(space) == -1 ) return 0;
		    rez = rez.substr(rez.indexOf(space)+space.length);
		  }
		  lstS = cfg.charAt(i); lenS = 0; i++; continue;
		}
		lenS++; i++;
	      }
	      if( this.value.indexOf('pm') >= 0 || this.value.indexOf('PM') >= 0 ) dt.setHours(dt.getHours()+12);
	      return dt.getTime()/1000;
	  }
	}
	formObj.chApply = function( )
	{
	  var val = this.valGet();
	  this.valSet(val);
	  var okImg = this.parentNode.childNodes[(this.parentNode.view>=1&&this.parentNode.view<=3)?2:1];
	  this.setModify(false);
	  var attrs = new Object(); attrs.value = val; attrs.event = 'ws_LnAccept';
	  setWAttrs(this.wdgLnk.addr,attrs);
	}
	formObj.chEscape = function( )
	{
	  this.value = this.saveVal;
	  var okImg = this.parentNode.childNodes[(this.parentNode.view>=1&&this.parentNode.view<=3)?2:1];
	  this.setModify(false);
	}
	var okImg = this.place.ownerDocument.createElement('img');
	okImg.src = '/'+MOD_ID+'/img_button_ok';
	okImg.style.cssText = 'visibility: hidden; position: absolute; left: '+(geomW-16)+'px; top: '+((geomH-16)/2)+'px; width: 16px; height: 16px; cursor: pointer;';
	okImg.onclick = function() { this.parentNode.childNodes[0].chApply(); return false; };
	this.place.appendChild(okImg);
	formObj.valSet(this.attrs['value']);
	break;
      case 1:	//Text edit
	var formObj = this.place.ownerDocument.createElement('textarea');
	formObj.disabled = !( parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR );
	formObj.wdgLnk = this;
	formObj.tabIndex = parseInt(this.attrs['geomZ'])+1;
	formObj.style.cssText = 'width: '+(geomW-5)+'px; height: '+(geomH-5)+'px; border: 1px solid black; font: '+fontCfg+'; padding: 1px;';
	formObj.appendChild(this.place.ownerDocument.createTextNode(this.attrs['value']));
	formObj.saveVal = formObj.value = this.attrs['value'];
	formObj.onkeyup = function( ) { if( this.saveVal != this.value ) this.setModify(true); };
	formObj.modify = function( ) { return (this.parentNode.childNodes[1].style.visibility == 'visible'); }
	formObj.setModify = function( on )
	{
	  if( this.modify() == on ) return;
	  if( on ) { this.style.height = (parseInt(this.style.height)-16)+'px'; this.parentNode.childNodes[1].style.visibility = this.parentNode.childNodes[2].style.visibility = 'visible'; }
	  else     { this.style.height = (parseInt(this.style.height)+16)+'px'; this.parentNode.childNodes[1].style.visibility = this.parentNode.childNodes[2].style.visibility = 'hidden'; }
	}
	var okImg = this.place.ownerDocument.createElement('img');
	okImg.src = '/'+MOD_ID+'/img_button_ok';
	okImg.style.cssText = 'visibility: hidden; position: absolute; left: '+(geomW-35)+'px; top: '+(geomH-16)+'px; width: 16px; height: 16px; cursor: pointer;';
	okImg.onclick = function() { var attrs = new Object(); attrs.value = this.parentNode.childNodes[0].value; attrs.event = 'ws_TxtAccept'; setWAttrs(this.parentNode.childNodes[0].wdgLnk.addr,attrs); this.parentNode.childNodes[0].setModify(false); return false; };
	var cancelImg = this.place.ownerDocument.createElement('img');
	cancelImg.src = '/"MOD_ID"/img_button_cancel';
	cancelImg.style.cssText = 'visibility: hidden; position: absolute; left: '+(geomW-16)+'px; top: '+(geomH-16)+'px; width: 16px; height: 16px; cursor: pointer;';
	cancelImg.onclick = function() { this.parentNode.childNodes[0].value = this.parentNode.childNodes[0].saveVal; this.parentNode.childNodes[0].setModify(false); return false; };
	this.place.appendChild(formObj); this.place.appendChild(okImg); this.place.appendChild(cancelImg);
	break;
      case 2:	//Chek box
	var tblCell = this.place.ownerDocument.createElement('div');
	tblCell.style.cssText = 'position: absolute; top: '+((geomH-15)/2)+'px; width: '+geomW+'px; height: '+Math.min(geomH,15)+'px; text-align: left; font: '+fontCfg+'; ';
	var formObj = this.place.ownerDocument.createElement('input');
	formObj.disabled = !( parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR );
	formObj.tabIndex = parseInt(this.attrs['geomZ'])+1;
	formObj.type='checkbox'; formObj.checked=parseInt(this.attrs['value']);
	formObj.wdgLnk = this;
	formObj.onclick = function( ) { var attrs = new Object(); attrs.value = (this.checked)?'1':'0'; attrs.event = 'ws_ChkChange'; setWAttrs(this.wdgLnk.addr,attrs); return true; }
	tblCell.appendChild(formObj); tblCell.appendChild(this.place.ownerDocument.createTextNode(this.attrs['name'])); this.place.appendChild(tblCell);
	break;
      case 3:	//Button
	var formObj;
	var iconImg = this.attrs['img'];
	this.place.checkable = parseInt(this.attrs['checkable']);
	if( iconImg || this.place.checkable )
	{
	  formObj = this.place.ownerDocument.createElement('div');
	  formObj.className = 'vertalign';
	  formObj.style.font = fontCfg;
	  elStyle+='border-style: '+((this.place.checkable && parseInt(this.attrs['value']))?'inset':'outset')+'; cursor: pointer; border-width: 2px; ';
	  if( this.attrs['colorText'] ) elStyle+='color: '+this.attrs['colorText']+'; ';
	  if( this.attrs['color'] ) elStyle+='background-color: '+this.attrs['color']+'; ';
	  else elStyle+='background-color: snow; ';
	  if( parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR )
	  {
	    this.place.onmouseup  = function() { if( !this.checkable ) this.style.borderStyle='outset'; };
	    this.place.onmousedown= function() { if( !this.checkable ) this.style.borderStyle='inset';  };
	    this.place.onmouseout = function() { if( !this.checkable ) this.style.borderStyle='outset'; };
	    this.place.onclick = function()
	    {
	      if( !this.checkable ) setWAttrs(this.wdgLnk.addr,'event','ws_BtPress');
	      else {
	        var attrs = new Object();
	        if(this.style.borderLeftStyle=='outset') { attrs.value = '1'; this.style.borderStyle='inset'; }
	        else { attrs.value = '0'; this.style.borderStyle='outset'; }
	        attrs.event = 'ws_BtToggleChange'; setWAttrs(this.wdgLnk.addr,attrs);
	      }
	      return false;
	    };
	    this.place.wdgLnk = this;
	  }
	  if( iconImg )
	  {
	    var btImg = this.place.ownerDocument.createElement('IMG');
	    btImg.src = '/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['img'];
	    btImg.width = btImg.height = Math.min(geomW-8,geomH-8);
	    btImg.float = 'left';
	    formObj.appendChild(btImg);
	  }
	  if( this.attrs['name'].length ) formObj.appendChild(this.place.ownerDocument.createTextNode(this.attrs['name']));
	  geomW-=6; geomH-=6;
	}
	else
	{
	  formObj = this.place.ownerDocument.createElement('input');
	  formObj.disabled = !( parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR );
	  formObj.tabIndex = parseInt(this.attrs['geomZ'])+1;
	  formObj.style.font = fontCfg;
	  formObj.type='button';
	  formObj.value=this.attrs['name'];
	  formObj.onclick = function() { setWAttrs(this.wdgLnk.addr,'event','ws_BtPress'); return false; }
	  formObj.wdgLnk = this;
	  if( this.attrs['color'] ) formObj.style.backgroundColor=this.attrs['color'];
	  if( this.attrs['colorText'] ) formObj.style.color=this.attrs['colorText'];
	}
	formObj.style.width = geomW+'px'; formObj.style.height = geomH+'px';
	this.place.appendChild(formObj);
	break;
      case 4: case 5:	//Combo box, List
	var formObj = this.place.ownerDocument.createElement('select');
	formObj.disabled = !( parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR );
	formObj.tabIndex = parseInt(this.attrs['geomZ'])+1;
	formObj.style.cssText = 'position: absolute; top: '+((elTp==4)?(geomH-20)/2:0)+'px; height: '+((elTp==4)?Math.min(geomH,20):(geomH-4))+'px; width: '+(geomW-4)+'px; border: 1px solid black; font: '+fontCfg+'; padding: 1px; ';
	formObj.wdgLnk = this;
//	if( elTp == 5 ) formObj.setAttribute('size',100);
	if( elTp == 4 )
	  formObj.onchange = function( ) { var attrs = new Object(); attrs.value = this.options[this.selectedIndex].value; attrs.event = 'ws_CombChange'; setWAttrs(this.wdgLnk.addr,attrs); }
	else
	{
	  formObj.size = 100;
	  formObj.onchange = function( ) { var attrs = new Object(); attrs.value = this.options[this.selectedIndex].value; attrs.event = 'ws_ListChange'; setWAttrs(this.wdgLnk.addr,attrs); }
	}
	var selVal = this.attrs['value'];
	var elLst = this.attrs['items'].split('\n');
	var selOk = false;
	for( var i = 0; i < elLst.length; i++ )
	{
	  var optEl = this.place.ownerDocument.createElement('option');
	  optEl.appendChild(this.place.ownerDocument.createTextNode(elLst[i]));
	  if( selVal == elLst[i] ) selOk=optEl.defaultSelected=optEl.selected=true;
	  formObj.appendChild(optEl);
	}
	if( !selOk && elTp == 4 )
	{
	  var optEl = this.place.ownerDocument.createElement('option');
	  optEl.textContent = selVal;
	  optEl.selected = optEl.defaultSelected = true;
	  formObj.appendChild(optEl);
	}
	this.place.appendChild(formObj);
	break;
    }
  }
  else if( this.attrs['root'] == 'Diagram' )
  {
    if( this.attrs['backColor'] ) elStyle+='background-color: '+this.attrs['backColor']+'; ';
    if( this.attrs['backImg'] )   elStyle+='background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['backImg']+'\'); ';
    elStyle+='border-style: solid; border-width: '+this.attrs['bordWidth']+'px; ';
    if( this.attrs['bordColor'] ) elStyle+='border-color: '+this.attrs['bordColor']+'; ';
    var anchObj = this.place.childNodes[0];
    if( !anchObj )
    {
      anchObj = this.place.ownerDocument.createElement('a');
      anchObj.wdgLnk = this;
      var dgrObj = this.place.ownerDocument.createElement('img');
      dgrObj.border = 0;
      anchObj.appendChild(dgrObj); this.place.appendChild(anchObj);
    }
    anchObj.isActive = (parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR);
    anchObj.href = '#';
    anchObj.tabIndex = parseInt(this.attrs['geomZ'])+1;
    anchObj.onfocus = function( ) { if(this.isActive) setFocus(this.wdgLnk.addr); }
    anchObj.onkeydown = function(e) { if(this.isActive) setWAttrs(this.wdgLnk.addr,'event','key_pres'+evKeyGet(e?e:window.event)); }
    anchObj.onkeyup = function(e) { if(this.isActive) setWAttrs(this.wdgLnk.addr,'event','key_rels'+evKeyGet(e?e:window.even)); }
    anchObj.onclick = function(e)
    {
      if( !this.isActive ) return false;
      if(!e) e = window.event;
      servSet(this.wdgLnk.addr,'com=obj&sub=point&x='+(e.offsetX?e.offsetX:(e.clientX-posGetX(this)))+
						'&y='+(e.offsetY?e.offsetY:(e.clientY-posGetY(this)))+
						'&key='+evMouseGet(e),'');
      setFocus(this.wdgLnk.addr);
      return false;
    }
    var dgrObj = anchObj.childNodes[0];
    dgrObj.src = '/'+MOD_ID+this.addr+'?com=obj&tm='+tmCnt+'&xSc='+xSc.toFixed(2)+'&ySc='+ySc.toFixed(2);
    this.perUpdtEn( this.isEnabled() && parseInt(this.attrs['trcPer']) );
  }
  else if( this.attrs['root'] == 'Protocol' )
  {
    if( this.attrs['backColor'] ) elStyle += 'background-color: '+this.attrs['backColor']+'; ';
    else elStyle+='background-color: white; ';
    if( this.attrs['backImg'] )   elStyle += 'background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['backImg']+'\'); ';
    elStyle += 'border: 1px solid black; overflow: auto; padding: 2px; ';
    geomW -= 4; geomH -= 4;

    this.wFont = '';
    var allFnt = this.attrs['font'].split(' ');
    if( allFnt.length >= 1 ) this.wFont+='font-family: '+allFnt[0].replace(/_/g,' ')+'; ';
    if( allFnt.length >= 2 ) this.wFont+='font-size: '+(parseInt(allFnt[1])*Math.min(xSc,ySc)).toFixed(0)+'px; ';
    if( allFnt.length >= 3 ) this.wFont+='font-weight: '+(parseInt(allFnt[2])?'bold':'normal')+'; ';
    if( allFnt.length >= 4 ) this.wFont+='font-style: '+(parseInt(allFnt[3])?'italic':'normal')+'; ';

    if( !this.place.firstChild )
    {
      this.place.appendChild(document.createElement('table'));
      this.place.firstChild.wdgLnk = this;
      this.place.firstChild.isActive = (parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR);
      this.place.firstChild.onclick = function(e)
      {
        if( this.isActive ) setFocus(this.wdgLnk.addr);
        return false;
      }
      this.place.firstChild.className='prot';
      this.loadData = function( )
      {
        var tblB = this.place.firstChild;

	//> Check for columns present and order
	if( !this.curCols || this.curCols != this.attrs['col'] || this.curArch != this.attrs['arch'] || this.curTmpl != this.attrs['tmpl'] || this.curLev != this.attrs['lev'] )
	{
	  this['col_pos'] = this['col_tm'] = this['col_utm'] = this['col_lev'] = this['col_cat'] = this['col_mess'] = -1;
	  this.curCols = this.attrs['col'];
	  this.curArch = this.attrs['arch'];
	  this.curTmpl = this.attrs['tmpl'];
	  this.curLev = this.attrs['lev'];
	  var rowEl = document.createElement('tr');
	  tblB.appendChild(rowEl);
	  var colCfg = '';
	  var clm = this.curCols.split(';');
	  for( var c_off = 0; c_off < clm.length; c_off++ )
	    if( clm[c_off] == 'pos' || clm[c_off] == 'tm' || clm[c_off] == 'utm' || clm[c_off] == 'lev' || clm[c_off] == 'cat' || clm[c_off] == 'mess' )
	    {
	      colCfg += "<th ind='"+clm[c_off]+"' "+
			  "style='"+this.wFont+"'>"+((clm[c_off]=='pos') ? '#' :
						    (clm[c_off]=='tm') ? '###Time###' :
						    (clm[c_off]=='utm') ? '###mcsec###' :
						    (clm[c_off]=='lev') ? '###Level###' :
						    (clm[c_off]=='cat') ? '###Category###' :
						    (clm[c_off]=='mess') ? '###Message###' : '')+"</th>";
	      this['col_'+clm[c_off]] = c_off;
	    }
	  rowEl.innerHTML = colCfg;
	  this.arhBeg = this.arhEnd = 0;
	}

	//> Get archive parameters
	var tTime = parseInt(this.attrs['time']);
	var tTimeGrnd = tTime - parseInt(this.attrs['tSize']);

	if( !this.arhBeg || !this.arhEnd || !tTime || tTime > this.arhEnd )
	{
	  var rez = servSet('/Archive/%2fserv%2fmess','com=com',"<info arch='"+this.curArch+"'/>",true);
	  if( !rez || parseInt(rez.getAttribute('rez')) != 0 ) { this.arhBeg = this.arhEnd = 0; }
	  else
	  {
	    this.arhBeg = parseInt(rez.getAttribute('beg'));
	    this.arhEnd = parseInt(rez.getAttribute('end'));
	    if( !tTime ) { tTime = this.arhEnd; tTimeGrnd += tTime; }
	  }
	}
	if( tblB.firstChild.childNodes.length <= 1 || !this.arhBeg || !this.arhEnd ) return;

	//> Correct request to archive border
	tTime = Math.min(tTime,this.arhEnd); tTimeGrnd = Math.max(tTimeGrnd,this.arhBeg);
	//> Clear data at time error
	var valEnd = 0; var valBeg = 0;
	if( tblB.childNodes.length>1 && tblB.firstChild.childNodes.length )
	{
	  while( tblB.childNodes.length>1 && (valEnd=parseInt(tblB.childNodes[1].childNodes[0].getAttribute('time'))) > tTime )
	    tblB.removeChild(tblB.childNodes[1]);
	  while( tblB.childNodes.length>1 && (valBeg=parseInt(tblB.lastChild.childNodes[0].getAttribute('time'))) < tTimeGrnd )
	    tblB.removeChild(tblB.lastChild);
	}

	if( tTime <= tTimeGrnd || (tTime < valEnd && tTimeGrnd > valBeg) )
	{
	  while( tblB.childNodes.length > 1 ) tblB.removeChild(tblB.lastChild);
	  valEnd = valBeg = 0;
	  return;
	}

	//> Correct request to present data
	var toUp = false;
	if( valEnd && tTime >= valEnd ) { tTimeGrnd = valEnd; toUp = true; }
	else if( valBeg && tTimeGrnd < valBeg ) tTime = valBeg-1;

	var rez = servSet('/Archive/%2fserv%2fmess','com=com',
	    "<get arch='"+this.curArch+"' tm='"+tTime+"' tm_grnd='"+tTimeGrnd+"' cat='"+this.curTmpl+"' lev='"+this.curLev+"' />",true);
	if( !rez || parseInt(rez.getAttribute('rez')) != 0 ) return;

	//> Process records
	if( toUp )
	  for( var i_req = 0; i_req < rez.childNodes.length; i_req++ )
	  {
	    var rcd = rez.childNodes[i_req];
	    var rtm = parseInt(rcd.getAttribute('time'));
	    var rutm = rcd.getAttribute('utime');
	    var rlev = rcd.getAttribute('lev');
	    var rcat = rcd.getAttribute('cat');
	    var rmess = nodeText(rcd);

	    //>> Check for dublicates
	    var i_p;
	    for( i_p = 1; i_p < tblB.childNodes.length; i_p++ )
	    {
	      if( rtm > parseInt(tblB.childNodes[1].childNodes[0].getAttribute('time')) && i_p>1 ) continue;
	      if( (this.col_utm<0 || nodeText(tblB.childNodes[i_p].childNodes[this.col_utm]) == rutm) &&
		    (this.col_lev<0 || nodeText(tblB.childNodes[i_p].childNodes[this.col_lev]) == rlev) &&
		    (this.col_cat<0 || nodeText(tblB.childNodes[i_p].childNodes[this.col_cat]) == rcat) &&
		    (this.col_mess<0 || nodeText(tblB.childNodes[i_p].childNodes[this.col_mess]) == rmess ) )
		break;
	    }
	    if( i_p < tblB.childNodes.length ) continue;

	    var rowEl = document.createElement('tr');
	    for( var i_cel = 0; i_cel < tblB.childNodes[0].childNodes.length; i_cel++ )
	    {
	      var celEl = document.createElement('td'); celEl.style.cssText = this.wFont;
	      if( i_cel == 0 ) celEl.setAttribute('time',rtm);
	      if( this.col_pos == i_cel ) { setNodeText(celEl,0); celEl.style.cssText+=' text-align: center; '; }
	      else if( this.col_tm == i_cel )
	      {
		var dt = new Date(rtm*1000);
		setNodeText(celEl,dt.getDate()+'.'+(dt.getMonth()+1)+'.'+dt.getFullYear()+' '+dt.getHours()+':'+
				  ((dt.getMinutes()<10)?('0'+dt.getMinutes()):dt.getMinutes())+':'+((dt.getSeconds()<10)?('0'+dt.getSeconds()):dt.getSeconds()));
	      }
	      else if( this.col_utm == i_cel ) setNodeText(celEl,rutm);
	      else if( this.col_lev == i_cel ) setNodeText(celEl,rlev);
	      else if( this.col_cat == i_cel ) setNodeText(celEl,rcat);
	      else if( this.col_mess == i_cel ) setNodeText(celEl,rmess);
	      rowEl.appendChild(celEl);
	    }
	    tblB.insertBefore(rowEl,tblB.childNodes[1]);
	  }
	else
	  for( var i_req = rez.childNodes.length-1; i_req >= 0; i_req-- )
	  {
	    var rcd = rez.childNodes[i_req];
	    var rtm = parseInt(rcd.getAttribute('time'));
	    var rutm = rcd.getAttribute('utime');
	    var rlev = rcd.getAttribute('lev');
	    var rcat = rcd.getAttribute('cat');
	    var rmess = nodeText(rcd);

	    //>> Check for dublicates
	    var i_p;
	    for( i_p = tblB.childNodes.length-1; i_p >= 1; i_p-- )
	    {
	      if( rtm < parseInt(tblB.childNodes[1].childNodes[0].getAttribute('time')) && (i_p<(tblB.childNodes.length-1)) ) continue;
	      if( (this.col_utm<0 || nodeText(tblB.childNodes[i_p].childNodes[this.col_utm]) == rutm) &&
		    (this.col_lev<0 || nodeText(tblB.childNodes[i_p].childNodes[this.col_lev]) == rlev) &&
		    (this.col_cat<0 || nodeText(tblB.childNodes[i_p].childNodes[this.col_cat]) == rcat) &&
		    (this.col_mess<0 || nodeText(tblB.childNodes[i_p].childNodes[this.col_mess]) == rmess) )
		break;
	    }
	    if( i_p >= 1 ) continue;

	    var rowEl = document.createElement('tr');
	    for( var i_cel = 0; i_cel < tblB.childNodes[0].childNodes.length; i_cel++ )
	    {
	      var celEl = document.createElement('td'); celEl.style.cssText = this.wFont;
	      if( i_cel == 0 ) celEl.setAttribute('time',rtm);
	      if( this.col_pos == i_cel ) { setNodeText(celEl,0); celEl.style.cssText+=' text-align: center; '; }
	      else if( this.col_tm == i_cel )
	      {
		var dt = new Date(rtm*1000);
		setNodeText(celEl,dt.getDate()+'.'+(dt.getMonth()+1)+'.'+dt.getFullYear()+' '+dt.getHours()+':'+
				  ((dt.getMinutes()<10)?('0'+dt.getMinutes()):dt.getMinutes())+':'+((dt.getSeconds()<10)?('0'+dt.getSeconds()):dt.getSeconds()));
	      }
	      else if( this.col_utm == i_cel ) setNodeText(celEl,rutm);
	      else if( this.col_lev == i_cel ) setNodeText(celEl,rlev);
	      else if( this.col_cat == i_cel ) setNodeText(celEl,rcat);
	      else if( this.col_mess == i_cel ) setNodeText(celEl,rmess);
	      rowEl.appendChild(celEl);
	    }
	    tblB.appendChild(rowEl);
	  }
	//> Update position collumn
	if( this.col_pos >= 0 )
	  for( var i_rw = 1; i_rw < tblB.childNodes.length; i_rw++ )
	    setNodeText(tblB.childNodes[i_rw].childNodes[this.col_pos],i_rw);
      }
    }

    if( this.isEnabled() && parseInt(this.attrs['trcPer']) ) this.perUpdtEn( true );
    else { this.perUpdtEn( false ); this.loadData(); }
  }
  else if( this.attrs['root'] == 'Document' )
  {
    elStyle+='background-color: white; ';

    this.wFont = '';
    var allFnt = this.attrs['font'].split(' ');
    if( allFnt.length >= 1 ) this.wFont+='font-family: '+allFnt[0].replace(/_/g,' ')+'; ';
    if( allFnt.length >= 2 ) this.wFont+='font-size: '+(parseInt(allFnt[1])*Math.min(xSc,ySc)).toFixed(0)+'px; ';
    if( allFnt.length >= 3 ) this.wFont+='font-weight: '+(parseInt(allFnt[2])?'bold':'normal')+'; ';
    if( allFnt.length >= 4 ) this.wFont+='font-style: '+(parseInt(allFnt[3])?'italic':'normal')+'; ';

    var ifrmObj = this.place.childNodes[0];
    if( !ifrmObj )
    {
      ifrmObj = this.place.ownerDocument.createElement('iframe');
      this.place.appendChild(ifrmObj);
    }
    ifrmObj.style.cssText = 'width: '+(geomW-14)+'px; height: '+(geomH-14)+'px; border-style: ridge; border-width: 2px; padding: 5px;';
    this.perUpdtEn(true);
  }
  elStyle+='width: '+geomW+'px; height: '+geomH+'px; z-index: '+this.attrs['geomZ']+'; margin: '+elMargin+'px; ';
  this.place.style.cssText = elStyle;
  }
  if( margBrdUpd ) for( var i in this.wdgs ) i.makeEl();
  this.place.setAttribute('title',this.attrs['tipTool']);
  this.place.onmouseover = function() { if( this.wdgLnk.attrs['tipStatus'] ) setStatus(this.wdgLnk.attrs['tipStatus'],10000); };

  //> Context menu setup
  if( parseInt(this.attrs['perm'])&(SEQ_RD|SEQ_WR) && parseInt(this.attrs['active']) && this.attrs['contextMenu'].length )
  {
    var ctxEv = function(e)
    {
      if( !e ) e = window.event;
      if( (isKonq || isOpera) && evMouseGet(e) != 'Right' ) return true;
      var popUpMenu = getPopup();
      var optEl = '';
      var cntxEls = this.wdgLnk.attrs['contextMenu'].split('\n');
      for( var i_ce = 0; i_ce < cntxEls.length; i_ce++ )
	optEl += "<option sign='"+cntxEls[i_ce].split(':')[1]+"'>"+cntxEls[i_ce].split(':')[0]+"</option>";
      popUpMenu.childNodes[0].innerHTML = optEl;
      if( popUpMenu.childNodes[0].childNodes.length )
      {
	popUpMenu.childNodes[0].wdgLnk = this.wdgLnk;
	popUpMenu.childNodes[0].size = Math.max(3,popUpMenu.childNodes[0].childNodes.length);
	popUpMenu.style.cssText = 'visibility: visible; left: '+(e.clientX+window.pageXOffset)+'px; top: '+(e.clientY+window.pageYOffset)+'px;';
	popUpMenu.childNodes[0].selectedIndex = -1;
	popUpMenu.childNodes[0].onclick = function()
	{
	    this.parentNode.style.cssText = 'visibility: hidden; left: -200px; top: -200px;';
	    if( this.selectedIndex < 0 ) return false;
	    setWAttrs(this.wdgLnk.addr,'event','usr_'+this.options[this.selectedIndex].getAttribute('sign'));
	    return false;
	}
      }
      return false;
    }
    if( isKonq || isOpera ) this.place.onmousedown = ctxEv;
    else this.place.oncontextmenu = ctxEv;
  }
  else this.place.oncontextmenu = this.place.onmousedown = null;

  //> Child widgets process
  if( pgBr && !inclPg && parseInt(this.attrs['perm'])&SEQ_RD )
    for( var j = 0; j < pgBr.childNodes.length; j++ )
    {
      if( pgBr.childNodes[j].nodeName != 'w' ) continue;
      var chEl = pgBr.childNodes[j].getAttribute('id');
      if( this.wdgs[chEl] ) this.wdgs[chEl].makeEl(pgBr.childNodes[j]);
      else
      {
        var wdgO = new pwDescr(this.addr+'/wdg_'+chEl,false,this);
        wdgO.place = this.place.ownerDocument.createElement('div');
        wdgO.makeEl(pgBr.childNodes[j]);
        this.place.appendChild(wdgO.place);
        this.wdgs[chEl] = wdgO;
      }
    }
}
function perUpdtEn( en )
{
  if( this.attrs['root'] == 'Diagram' || this.attrs['root'] == 'Protocol' )
  {
     if( en && this.isEnabled() && !perUpdtWdgs[this.addr] && parseInt(this.attrs['trcPer']) ) perUpdtWdgs[this.addr] = this;
     if( !en && perUpdtWdgs[this.addr] ) delete perUpdtWdgs[this.addr];
  }
  else if( this.attrs['root'] == 'Document' ) { if(en) perUpdtWdgs[this.addr] = this; else delete perUpdtWdgs[this.addr]; }
  for( var i in this.wdgs ) this.wdgs[i].perUpdtEn(en);
}
function perUpdt( )
{
  if( this.attrs['root'] == 'Diagram' && (this.updCntr % parseInt(this.attrs['trcPer'])) == 0 )
  {
    var dgrObj = this.place.childNodes[0].childNodes[0];
    if( dgrObj ) dgrObj.src = '/'+MOD_ID+this.addr+'?com=obj&tm='+tmCnt+'&xSc='+this.xScale(true).toFixed(2)+'&ySc='+this.yScale(true).toFixed(2);
  }
  else if( this.attrs['root'] == 'Protocol' && (this.updCntr % parseInt(this.attrs['trcPer'])) == 0 ) this.loadData();
  else if( this.attrs['root'] == 'Document' )
  {
    var frDoc = this.place.childNodes[0].contentDocument || this.place.childNodes[0].contentWindow || this.place.childNodes[0].document;
    frDoc.open();
    frDoc.write("<html><head>"+
		"<style type='text/css'>"+
		" * { "+this.wFont+" }\n"+
		" big { font-size: 120%; }\n"+
		" small { font-size: 90%; }\n"+
		" h1 { font-size: 200%; }\n"+
		" h2 { font-size: 150%; }\n"+
		" h3 { font-size: 120%; }\n"+
		" h4 { font-size: 105%; }\n"+
		" h5 { font-size: 95%; }\n"+
		" h6 { font-size: 70%; }\n"+
		" u,b,i { font-size : inherit; }\n"+
		" sup,sub { font-size: 80%; }\n"+
		this.attrs['style']+"</style>"+
		"</head>"+(this.attrs['doc']?this.attrs['doc']:this.attrs['tmpl'])+"</html>");
    frDoc.close();
    frDoc.wdgLnk = this;
    frDoc.isActive = (parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEQ_WR);
    frDoc.onclick = function(e)
    {
      if( this.isActive ) setFocus(this.wdgLnk.addr);
      return false;
    }
    this.perUpdtEn( false );
  }
  this.updCntr++;
}
function xScale( full )
{
  var rez = parseFloat(this.attrs['geomXsc'])
  if( !full ) return rez;
  if( !this.pg ) return rez*this.parent.xScale(full);
  if( this != masterPage ) return masterPage.xScale()*rez;
  return rez;
}
function yScale( full )
{
  var rez = parseFloat(this.attrs['geomYsc'])
  if( !full ) return rez;
  if( !this.pg ) return rez*this.parent.yScale(full);
  if( this != masterPage ) return masterPage.yScale()*rez;
  return rez;
}
function isEnabled( )
{
  var rez = parseInt(this.attrs['en']);
  if( !rez || this.pg ) return rez;
  return this.parent.isEnabled();
}
function pwDescr( pgAddr, pg, parent )
{
  this.addr = pgAddr;
  this.pages = new Object();
  this.wdgs = new Object();
  this.attrs = new Object();
  this.pg = pg;
  this.parent = parent;
  this.window = null;
  this.place = null;
  this.callPage = callPage;
  this.findOpenPage = findOpenPage;
  this.makeEl = makeEl;
  this.perUpdtEn = perUpdtEn;
  this.perUpdt = perUpdt;
  this.xScale = xScale;
  this.yScale = yScale;
  this.isEnabled = isEnabled;
  this.updCntr = 0;
}
/***************************************************
 * makeUI                                          *
 ***************************************************/
function makeUI()
{
  //> Get open pages list
  var pgNode = servGet('/'+sessId,'com=pgOpenList&tm='+tmCnt);
  if( pgNode )
  {
    //>> Check for delete pages
    for( var i_p = 0; i_p < pgList.length; i_p++ )
    {
      var opPg; var i_ch;
      for( i_ch = 0; i_ch < pgNode.childNodes.length; i_ch++ )
        if( pgNode.childNodes[i_ch].nodeName == 'pg' && nodeText(pgNode.childNodes[i_ch]) == pgList[i_p] )
          break;
      if( i_ch < pgNode.childNodes.length || !(opPg=masterPage.findOpenPage(pgList[i_p])) ) continue;
      if( opPg.window ) { opPg.window.close(); delete opPg.parent.pages[pgList[i_p]]; }
      else if( opPg.parent && opPg.parent.inclOpen && opPg.parent.inclOpen == pgList[i_p] )
      { opPg.parent.attrs['pgOpenSrc'] = ''; opPg.parent.makeEl(null,true); }
    }
    //>> Process opened pages
    pgList = new Array();
    for( var i = 0; i < pgNode.childNodes.length; i++ )
      if( pgNode.childNodes[i].nodeName == 'pg' )
      {
        var prPath = nodeText(pgNode.childNodes[i]);
        //>>> Check for closed window
        var opPg = masterPage.findOpenPage(prPath);
        if( opPg && opPg.window && opPg.window.closed )
        { servSet(prPath,'com=pgClose',''); delete opPg.parent.pages[prPath]; continue; }
        //>>> Call page
        pgList.push(prPath);
        masterPage.callPage(prPath,parseInt(pgNode.childNodes[i].getAttribute('updWdg')));
      }
    tmCnt = parseInt(pgNode.getAttribute('tm'));
  }
  //> Update some widgets
  for( var i in perUpdtWdgs ) perUpdtWdgs[i].perUpdt();
  setTimeout(makeUI,1000);
}

/***************************************************
 * setStatus - Setup status message.               *
 ***************************************************/
function setStatus( mess, tm )
{
  window.status = mess ? mess : '###Ready###';
  if( !mess ) return;
  if( stTmID ) clearTimeout(stTmID);
  if( !tm || tm > 0 ) stTmID = setTimeout('setStatus(null)',tm?tm:1000);
}

/**************************************************
 * getPopup - Get popup menu.                     *
 **************************************************/
function getPopup( )
{
  var popUpMenu = document.getElementById('popupmenu');
  if( !popUpMenu )
  {
    popUpMenu = document.createElement('div'); popUpMenu.id = 'popupmenu';
    popUpMenu.appendChild(document.createElement('select'));
    document.body.appendChild(popUpMenu);
    popUpMenu.style.visibility = 'hidden';
  }
  return popUpMenu;
}

/***************************************************
 * Main start code                                 *
 ***************************************************/
SEQ_XT = 0x01;	//Extended
SEQ_WR = 0x02;	//Write access
SEQ_RD = 0x04;	//Read access
//Call session identifier
sessId = location.pathname.split('/');
for( var i_el = sessId.length-1; i_el >= 0; i_el-- )
  if( sessId[i_el].length )
  { sessId = sessId[i_el]; break; }

document.body.onmouseup = function(e)
{
  if( !e ) e = window.event;
  if( evMouseGet(e) != 'Left' ) return true;
  var popUpMenu = document.getElementById('popupmenu');
  if( popUpMenu ) popUpMenu.style.visibility = 'hidden';
}

tmCnt = 0;				//Call counter
pgList = new Array();			//Opened pages list
pgCache = new Object();			//Cached pages' data
perUpdtWdgs = new Object();		//Periodic updated widgets register
masterPage = new pwDescr('',true);	//Master page create
stTmID = null;				//Status line timer identifier

setTimeout(makeUI,1000);		//First call init
