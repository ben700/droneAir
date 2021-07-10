
var theText = "Droneponics Device Manager";
for (i = 0; i < theText.length; i++) {
  delay = (i * 3) / theText.length;
  var e = $("<span>" + theText[i] + "</span>");
  $("div").append(e);
  e.css("animation-delay", delay+"s");
}