function goto(inputField) {
	window.name = inputField.value;
	window.location.href = "http://localhost:8080/" + "fuckyou";
}

function initFuckyou() {
	const para = document.getElementById("intro-text");
	let text = window.name;
	para.innerHTML = `'${text}'????? What do you mean, '${text}'?????`;

	showHostName()
	navigator.geolocation.getCurrentPosition(showGeoLoc);
}

function showGeoLoc(position) {
	let para = document.createElement("p");
	para.innerHTML = `Location: ${position.coords.longitude}, ${position.coords.latitude}`;
	document.body.append(para);
}

function showHostName() {
	let para = document.createElement("p");
	para.innerHTML = `Name: Loser.png`;
	document.body.append(para);
}