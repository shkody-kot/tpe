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
var tpe_blocksize, tpe_blocksize_x, tpe_blocksize_y, tpe_iteration, tpe_key, tpe_file;
var tpe;
var encrypt;
var decrypt;
var create;
var m, n, total_for_perm, total_for_sub;

var init = function() {
	// document.getElementById("set").disabled = true;
	document.getElementById("decrypt").disabled = true;
	document.getElementById("encrypt").disabled = true;
	document.getElementById("iterations").value = 10;
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
				populate_dropdown(img.width, img.height);
			};
			img.src = e.target.result;
			//populate_dropdown(img.width, img.height);
		};
		FR.readAsDataURL(this.files[0]);
	}
	console.log("- browse");
};

var populate_dropdown = function(image_width, image_height)
{
	//for blocksize x
	possible_blocksizes = [];
	for (var i = 0; i < image_width; i++)
	{
		if (image_width % i == 0) { possible_blocksizes.push(i); }
	}
	var selection = document.getElementById("blocksizex");
	
	//remove all current children
	var child = selection.lastElementChild;
	while (child)
	{
		selection.removeChild(child);
		child = selection.lastElementChild;
	}
	
	console.log(possible_blocksizes);
	//add new children
	for (var i = 0; i < possible_blocksizes.length; i++)
	{
		var size = possible_blocksizes[i];
		console.log(size);
		var option = document.createElement("option");
		option.value = size;
		option.text = size;
		selection.appendChild(option);
	}
	
	//for blocksize y
	possible_blocksizes = [];
	for (var i = 0; i < image_height; i++)
	{
		if (image_height % i == 0) { possible_blocksizes.push(i); }
	}
	selection = document.getElementById("blocksizey");
	
	//remove all current children
	child = selection.lastElementChild;
	while (child)
	{
		selection.removeChild(child);
		child = selection.lastElementChild;
	}
	//add new children
	for (var i = 0; i < possible_blocksizes.length; i++)
	{
		var size = possible_blocksizes[i];
		console.log(size);
		var option = document.createElement("option");
		option.value = size;
		option.text = size;
		selection.appendChild(option);
	}
}

var set = function(){
	console.log("+ set");
	Module._free(tpe);
	tpe_iteration = document.getElementById("iterations").value;
	tpe_blocksize = document.getElementById("blocksizex").value * document.getElementById("blocksizey").value;
	tpe_blocksize_x = document.getElementById("blocksizex").value;
	tpe_blocksize_y = document.getElementById("blocksizey").value;
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
		let key;
		try
		{
			key = allocate(Module.intArrayFromString(tpe_key), 'i8', ALLOC_NORMAL);
			tpe = Module.__Z6createPciii(key, tpe_iteration, tpe_blocksize_x, tpe_blocksize_y);
			
		} finally
		{
			m = Math.floor(image_p.width / tpe_blocksize_x) | 0;
			n = Math.floor(image_p.height / tpe_blocksize_x) | 0;
			
			total_for_perm = tpe_iteration * n * m * tpe_blocksize;
			total_for_sub = tpe_iteration * n * m * (tpe_blocksize - tpe_blocksize % 2) / 2 * 3;
			
			console.log(tpe);
			Module._free(key);
			draw_thumbnail(image_p, image_pt)
		}
		document.getElementById("encrypt").disabled = false;
	}
};

var draw_thumbnail = function (icanvas, tcanvas) {
	console.log("+ draw_thumbnail");
	var img_data = icanvas.getContext("2d").getImageData(0, 0, icanvas.width, icanvas.height);
	var data = img_data.data;
	tcanvas.width = icanvas.width;
	tcanvas.height = icanvas.height;

	var m = parseInt(Math.floor(icanvas.width / tpe_blocksize_x));
	var n = parseInt(Math.floor(icanvas.height / tpe_blocksize_y));

	var r, g, b, p, q;
	//for each row of blocks
	for (var i = 0; i < n; i++) {
		//for each column of blocks
		for (var j = 0; j < m; j++) {
			r = 0;
			g = 0;
			b = 0;
			//for each block
			for (var k = 0; k < tpe_blocksize; k += 1) {
				p = parseInt(k / tpe_blocksize_x);		//row number
				q = k % tpe_blocksize_x;				//column number
				
				//row_num * height * block_height + pixel_row_num * width * 
				r += data[(i * icanvas.width * tpe_blocksize_y + p * icanvas.width + j * tpe_blocksize_x + q) * 4];
				g += data[(i * icanvas.width * tpe_blocksize_y + p * icanvas.width + j * tpe_blocksize_x + q) * 4 + 1];
				b += data[(i * icanvas.width * tpe_blocksize_y + p * icanvas.width + j * tpe_blocksize_x + q) * 4 + 2];
			}
			r = parseInt(r / (tpe_blocksize));
			g = parseInt(g / (tpe_blocksize));
			b = parseInt(b / (tpe_blocksize));
			console.log('red: ' + r + ' green: ' + g + ' blue: ' + b);
			for (var k = 0; k < tpe_blocksize; k += 1) {
				p = parseInt(k / tpe_blocksize_x);
				q = k % tpe_blocksize_x;
				data[(i * icanvas.width * tpe_blocksize_y + p * icanvas.width + j * tpe_blocksize_x + q) * 4] = r;
				data[(i * icanvas.width * tpe_blocksize_y + p * icanvas.width + j * tpe_blocksize_x + q) * 4 + 1] = g;
				data[(i * icanvas.width * tpe_blocksize_y + p * icanvas.width + j * tpe_blocksize_x + q) * 4 + 2] = b;
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
	
	console.log(image_data.data);
	
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
			console.log(decrypted);
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
