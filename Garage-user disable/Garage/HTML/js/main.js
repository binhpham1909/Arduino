function currentDateTime() {
	var d = new Date();
	var h = d.getHours();
	if ( h<10 ) h="0"+h;
	var m = d.getMinutes();
	if ( m<10 ) m="0"+m;
	var s = d.getSeconds();
	if ( s<10 ) s="0"+s;
	return (d.getMonth()+1)+"/"+d.getDate()+"/"+d.getFullYear()+" "+h+":"+m+":"+s;
}
function currentServerDateTime(epochServer,epochClient) {
	if ( !epochServer ) return "";
	var diffEpoch = (Date.now()-epochClient);
	var d = new Date(epochServer*1000+diffEpoch);
	var h = d.getUTCHours();
	if ( h<10 ) h="0"+h;
	var m = d.getUTCMinutes();
	if ( m<10 ) m="0"+m;
	var s = d.getUTCSeconds();
	if ( s<10 ) s="0"+s;
	return (d.getUTCMonth()+1)+"/"+d.getUTCDate()+"/"+d.getUTCFullYear()+" "+h+":"+m+":"+s;
}
var showMessageTimeoutId = 0;
function hideMessages() {
	clearTimeout(showMessageTimeoutId);
	$("#errorMsg").hide();
	$("#infoMsg").hide();
}
function showErrorMsg(msg) {
	var m = $("#errorMsg");
	m.html(msg);
	m.show();
	$("#infoMsg").hide();
	clearTimeout(showMessageTimeoutId);
	showMessageTimeoutId = setTimeout(function(){ hideMessages(); }, 7000);
}
function showInfoMsg(msg) {
	var m = $("#infoMsg");
	m.html(msg);
	m.show();
	$("#errorMsg").hide();
	clearTimeout(showMessageTimeoutId);
	showMessageTimeoutId = setTimeout(function(){ hideMessages(); }, 7000);
}
function isValidNumber(f,min,max) {
	var s = f.val();
	var re = /^[0-9]{1,5}$/;
	console.log(s);
	if ( !re.test(s) ) {
	  	f.focus();
	  	showErrorMsg("Error: Invalid number!");
	  	return false;
	}
	var n = parseInt(s);
	if ( n<min || n>max ) {
	  	f.focus();
	  	showErrorMsg("Error: Number must be in range: " + min + "-" + max);
	  	return false;
	}
	return true;
}
function isIPv4(f) {
	var ip = "" + f.val();
	if ( ip.length<1 ) {
		f.focus();
		return false;
	}
	var reg = /^([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})$/;
	var rsl = ip.match(reg);
	if ( !rsl || rsl.length<5 ) {
		f.focus();
		return false;
	}
	var i;
	for (i=1; i<5; i++) {
		var x = parseInt(rsl[i],10);
		if ( i<0 || i>255 ) {
			f.focus();
			return false;
		}
	}
	if ( rsl[1]<1 || rsl[4]<1 ) {
		f.focus();
		return false;
	}
	return true;
}
function isSubnetMask(f) {
	var ip = "" + f.val();
	if ( ip.length<1 ) {
		f.focus();
		return false;
	}
	var reg = /^(255)\.([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})$/;
	var rsl = ip.match(reg);
	if ( rsl===null || rsl.length<5 ) {
		f.focus();
		return false;
	}
	var i;
	for (i=1; i<5; i++) {
		var x = parseInt(rsl[i],10);
		if ( i<0 || i>255 ) {
			f.focus();
			return false;
		}
	}
	return true;
}
