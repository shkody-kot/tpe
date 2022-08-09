document.addEventListener('DOMContentLoaded', function () {
	init();
	// browse
	document.getElementById('browse').addEventListener('change', browse, false);
	// show
	document.getElementById('set').addEventListener('click', set, false);
	// encrypt
	document.getElementById('encrypt').addEventListener('click', encrypt, false);
	// decrypt
	document.getElementById('decrypt').addEventListener('click', decrypt, false);
});

var image_p, image_e, image_d;
var image_pt, image_et, image_dt;
var tpe_blocksize, tpe_iteration, tpe_key, tpe_file;
var tpe;
var encrypt;
var decrypt;
var create;

var init = function() {
	// document.getElementById("set").disabled = true;
	document.getElementById("decrypt").disabled = true;
	document.getElementById("encrypt").disabled = true;
	document.getElementById("iterations").value = 10;
	document.getElementById("blocksize").value = 20;
	document.getElementById("abc").hidden = true;
	document.getElementById("key").value = "V7a6kjqDeQUBNAev118sjOp3fbv_RMsHorWHkzuDCsM";
	console.log("128 bit key hard-coded for demo!");
	console.log(key.value);

	image_p = document.getElementById("canvas-og");
	image_pt = document.getElementById("canvas-ogt");
	image_e = document.getElementById("canvas-en");
	image_et = document.getElementById("canvas-ent");
	image_d = document.getElementById("canvas-de");
	image_dt = document.getElementById("canvas-det");
};

var browse = function () {
	console.log("+ browse");
	if (this.files && this.files[0]) {
		var FR = new FileReader();
		FR.onload = function (e) {
			var img = new Image();
			img.onload = function () {
				image_p = document.getElementById("canvas-og");
				image_p.width = img.width;
				image_p.height = img.height;
				image_p.getContext("2d").drawImage(img, 0, 0, image_p.width, image_p.height);
				image_pt = document.getElementById("canvas-ogt");
				image_pt.width = img.width;
				image_pt.height = img.height;
				image_e.width = img.width;
				image_e.height = img.height;
				image_et.width = img.width;
				image_et.height = img.height;
				image_d.width = img.width;
				image_d.height = img.height;
				image_dt.width = img.width;
				image_dt.height = img.height;
				console.log(img.width);
				console.log(img.height);
			};
			img.src = e.target.result;
		};
		FR.readAsDataURL(this.files[0]);
	}
	console.log("- browse");
};

const importObject = {
	imports: {
		imported_func(arg) {
			console.log(arg);
		},
	},
};

var set = function(){
	console.log("+ set");
	Module._free(tpe);
	tpe_iteration = document.getElementById("iterations").value;
	tpe_blocksize = document.getElementById("blocksize").value;
	tpe_key = document.getElementById("key").value;
	tpe_file = document.getElementById("browse").value

	if(tpe_blocksize == null || tpe_blocksize == "" ||
		tpe_blocksize == null || tpe_blocksize == "" ||
		tpe_file == null || tpe_file == "" ||
		tpe_key == null || tpe_key == ""){
			alert("Please set valid parameters!")
	}
	else
	{
		draw_thumbnail(image_p, image_pt)
		let key;
		try
		{
			key = allocate(Module.intArrayFromString(tpe_key), 'i8', ALLOC_NORMAL);
			tpe = Module.__Z6createPcii(key, tpe_iteration, tpe_blocksize);
			
		} finally
		{
			console.log(tpe);
			Module._free(key);
		}
		document.getElementById("encrypt").disabled = false;
	}
};

var draw_thumbnail = function (icanvas, tcanvas) {
	console.log("+ draw_thumbnail");
	var blocksize = tpe_blocksize;
	var img_data = icanvas.getContext("2d").getImageData(0, 0, icanvas.width, icanvas.height);
	var data = img_data.data;
	tcanvas.width = icanvas.width;
	tcanvas.height = icanvas.height;

	var m = parseInt(Math.floor(icanvas.width / blocksize));
	var n = parseInt(Math.floor(icanvas.height / blocksize));

	var r, g, b, p, q;
	for (var i = 0; i < n; i++) {
		for (var j = 0; j < m; j++) {
			r = 0;
			g = 0;
			b = 0;
			for (var k = 0; k < blocksize * blocksize; k += 1) {
				p = parseInt(k / blocksize);
				q = k % blocksize;
				r += data[(i * icanvas.width * blocksize + p * icanvas.width + j * blocksize + q) * 4];
				g += data[(i * icanvas.width * blocksize + p * icanvas.width + j * blocksize + q) * 4 + 1];
				b += data[(i * icanvas.width * blocksize + p * icanvas.width + j * blocksize + q) * 4 + 2];
			}
			for (var k = 0; k < blocksize * blocksize; k += 1) {
				p = parseInt(k / blocksize);
				q = k % blocksize;
				data[(i * icanvas.width * blocksize + p * icanvas.width + j * blocksize + q) * 4] = parseInt(r / (blocksize * blocksize));
				data[(i * icanvas.width * blocksize + p * icanvas.width + j * blocksize + q) * 4 + 1] = parseInt(g / (blocksize * blocksize));
				data[(i * icanvas.width * blocksize + p * icanvas.width + j * blocksize + q) * 4 + 2] = parseInt(b / (blocksize * blocksize));
			}
		}
	}
	var ctx = tcanvas.getContext("2d");
	ctx.putImageData(img_data, 0, 0);
	console.log("- draw_thumbnail");
};

var encrypt = function ()
{
	console.log("+ encrypt");
	var ctx = document.getElementById("canvas-og").getContext("2d");
	var ctx_out = document.getElementById("canvas-en").getContext("2d");
	var width = document.getElementById("canvas-og").width;
	var height = document.getElementById("canvas-og").height;
	var image_data = ctx.getImageData(0, 0, width, height);
	let sub_array = [];
	let perm_array = [];
	
	var m = Math.floor(width / tpe_blocksize) | 0,
        n = Math.floor(height / tpe_blocksize) | 0;
	
	let total_for_perm = tpe_iteration * n * m * tpe_blocksize * tpe_blocksize;
	let total_for_sub = tpe_iteration * n * m * (tpe_blocksize * tpe_blocksize - (tpe_blocksize * tpe_blocksize) % 2) / 2 * 3;
	
	(async () => {
		sub_array = await pseudo_rnd(tpe_key, total_for_sub);
		perm_array =  await pseudo_rnd(tpe_key, total_for_perm);
		
		let image, sub, perm, encrypted;
		try
		{
			image = allocate(image_data.data, 'i8', ALLOC_NORMAL);
			sub = allocate(sub_array, 'i8', ALLOC_NORMAL);
			perm = allocate(perm_array, 'i8', ALLOC_NORMAL);
			encrypted = Module.__Z7encryptPhS_S_iiP3tpe(image, sub, perm, width, height, tpe);
			encrypted = Module.HEAPU8.subarray(encrypted, encrypted + image_data.data.length);
			console.log(encrypted);
			for (var i = 0; i < image_data.data.length; i += 1) { image_data.data[i] = encrypted[i];  }
		}
		finally
		{
			Module._free(image);
			Module._free(sub);
			Module._free(perm);
			Module._free(encrypted);
		}
		
		ctx_out.putImageData(image_data, 0, 0);
		image_e = document.getElementById("canvas-en");
		image_et = document.getElementById("canvas-ent");
		draw_thumbnail(image_e, image_et);
	})();
	
	document.getElementById("decrypt").disabled = false;
};

var setUploadStatus = function (status) {
	var s = document.getElementById("upload_status");
	var msg = "Upload Status: " + status
	s.innerHTML = msg;
	console.log(msg)
};


var setUploadStat = function (stat) {
	var s = document.getElementById("upload_stat");
	s.innerHTML = stat;
	console.log("upload stat:" + stat)
};


var upload = function () {
	chrome.identity.getAuthToken({
		'interactive': true
	}, function (token) {
		var c = document.getElementById("canvas");
		setUploadStatus("uploading...");
		c.toBlob(function (blob) {
			sendImageBinary(token, blob)
		});
	});
};


var sendImageBinary = function (token, binaryData) {
	var time = new Date();
	var req = new XMLHttpRequest();
	req.open('POST', 'https://photoslibrary.googleapis.com/v1/uploads', true);
	req.setRequestHeader('Content-type', 'application/octet-stream');
	req.setRequestHeader('Authorization', 'Bearer ' + token);
	req.setRequestHeader('X-Goog-Upload-File-Name', time.toJSON());
	req.onreadystatechange = function () {
		var that = this;
		if (that.readyState === 4 && that.status === 200) {
			// ref https://developers.google.com/photos/library/guides/upload-media#uploading-bytes
			console.log("SUCCESS sendImageBinary");
			console.log(that.responseText);
			var uploadToken = this.response;
			setUploadStatus("Uploaded Image Binary");
			sendMediaItemRequest(token, uploadToken);
		} else {
			console.log("ERROR in sendImageBinary");
			setUploadStatus("Upload Failed at sendImageBinary");
		}
	};
	req.send(binaryData);
};


var sendMediaItemRequest = function (token, uploadToken) {
	var req = new XMLHttpRequest();
	req.open('POST', 'https://photoslibrary.googleapis.com/v1/mediaItems:batchCreate', true);
	req.setRequestHeader('Content-type', 'application/json');
	req.setRequestHeader('Authorization', 'Bearer ' + token);
	var payload = {
		"newMediaItems": [{
			"description": "tpeimage",
			"simpleMediaItem": {
				"uploadToken": uploadToken
			}
		}]
	}
	payload = JSON.stringify(payload);
	req.onreadystatechange = function () {
		var that = this;
		if (that.readyState === 4 && that.status === 200) {
			// ref https://developers.google.com/photos/library/guides/upload-media#item-creation-response
			setUploadStatus("Upload Success");
			var response = JSON.parse(that.response);
			var url = response.newMediaItemResults[0].mediaItem.productUrl;
			setUploadStatus('<a href="' + url + '">Google Photos</a>');
		} else {
			setUploadStatus("Upload Failed at sendMediaItemRequest");
		}
	};
	req.send(payload);
};

var decrypt = function ()
{
	console.log("+ decrypt");
	var ctx = document.getElementById("canvas-en").getContext("2d");
	var ctx_out = document.getElementById("canvas-de").getContext("2d");
	var width = document.getElementById("canvas-en").width;
	var height = document.getElementById("canvas-en").height;
	var image_data = ctx.getImageData(0, 0, width, height);
	let sub_array = [];
	let perm_array = [];
	
	var m = Math.floor(width / tpe_blocksize) | 0,
        n = Math.floor(height / tpe_blocksize) | 0;
	
	let total_for_perm = tpe_iteration * n * m * tpe_blocksize * tpe_blocksize;
	let total_for_sub = tpe_iteration * n * m * 
		(tpe_blocksize * tpe_blocksize - (tpe_blocksize * tpe_blocksize) % 2) / 2 * 3;
	
	(async () => {
		sub_array = await pseudo_rnd(tpe_key, total_for_sub);
		perm_array =  await pseudo_rnd(tpe_key, total_for_perm);
		
		let image, sub, perm, decrypted;
		try
		{
			image = allocate(image_data.data, 'i8', ALLOC_NORMAL);
			sub = allocate(sub_array, 'i8', ALLOC_NORMAL);
			perm = allocate(perm_array, 'i8', ALLOC_NORMAL);
			decrypted = Module.__Z7decryptPhS_S_iiP3tpe(image, sub, perm, width, height, tpe);
			decrypted = Module.HEAPU8.subarray(decrypted, decrypted + image_data.data.length);
			for (var i = 0; i < image_data.data.length; i += 1) { image_data.data[i] = decrypted[i];  }
		}
		finally
		{
			Module._free(image);
			Module._free(sub);
			Module._free(perm);
			Module._free(decrypted);
			Module._free(tpe);
		}
		
		ctx_out.putImageData(image_data, 0, 0);
		image_d = document.getElementById("canvas-de");
		image_dt = document.getElementById("canvas-det");
		draw_thumbnail(image_d, image_dt);
	})();
};

var setDownloadStatus = function (status) {
	var s = document.getElementById("download_status");
	var msg = "Download Status: " + status;
	s.innerHTML = msg;
	console.log(msg);
};


var setDownloadStat = function (stat) {
	var s = document.getElementById("download_stat");
	s.innerHTML = stat;
	console.log("download stat:" + stat);
};


var download = function () {
	var downloadLink = document.getElementById("dl_link");
	var x = document.getElementById("canvas_dl");
	var dt = x.toDataURL('image/png');
	downloadLink.href = dt;
	downloadLink.download = "download";
	setDownloadStatus("Downloading...")
	downloadLink.click();
	setDownloadStatus("Done!");
};

async function pseudo_rnd(key, need)
{
	let data = new Uint8Array(need);
	var cipher_text;
	//returns Promise containing CryptoKey object
	try
	{
		const enc_key = await window.crypto.subtle.importKey(
			"jwk",
			{
				kty: "oct",
				k: key, 
				alg: "A256CTR",
				ext: true
			},
			{ name: "AES-CTR" },
			false,
			["encrypt", "decrypt"]
		);
		
		//encrypts the data (all 0's) and returns an array buffer
		cipher_text = await window.crypto.subtle.encrypt(
			{
				name: "AES-CTR",
				counter: new Uint8Array(16),
				length: 128,
			},
			enc_key,
			data
		);
	} finally 
	{
		data = new Uint8Array(cipher_text);
		return data;
	}
};
