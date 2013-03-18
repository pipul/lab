var aid = 1;

function delAttach(id) {
	$('attachbody').removeChild($('attach_' + id).parentNode.parentNode);
	$('attachbody').innerHTML == '' && addAttach();
}

function addAttach() {
	newnode = $('attachbodyhidden').firstChild.cloneNode(true);
	var id = aid;
	var tags;
	tags = newnode.getElementsByTagName('input');
	for(i in tags) {
		if(tags[i].name == 'attach[]') {
			tags[i].id = 'attach_' + id;
			tags[i].onchange = function() {insertAttach(id)};
			tags[i].unselectable = 'on';
		}
		if(tags[i].name == 'localid[]') {
			tags[i].value = id;
		}
	}
	tags = newnode.getElementsByTagName('span');
	for(i in tags) {
		if(tags[i].id == 'localfile[]') {
			tags[i].id = 'localfile_' + id;
		}
	}
	aid++;
	$('attachbody').appendChild(newnode);
}

addAttach();

function insertAttach(id) {
	var path = $('attach_' + id).value;
	var localfile = $('attach_' + id).value.substr($('attach_' + id).value.replace(/\\/g, '/').lastIndexOf('/') + 1);

	if(path == '') {
		return;
	}
	$('localfile_' + id).innerHTML = '[<a href="javascript:delAttach(' + id + ');">删除</a>] [<a href="###" onclick="insertAttachtext(' + id + ');return false;">插入</a>] [' + id + '] ' + localfile;
	$('attach_' + id).style.display = 'none';

	addAttach();
}

function insertAttachtext(id) {
	addhtml('[localfile=' + id + ']');
}