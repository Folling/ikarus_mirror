// maximum efficiency
function enumFormatFix() {
    Array.from(document.getElementsByClassName("memItemRight")).forEach((elem) => {
        elem.innerHTML = elem.innerHTML.replaceAll("<br>", "");
        elem.innerHTML = elem.innerHTML.replaceAll("&nbsp;", "");
        elem.innerHTML = elem.innerHTML.replaceAll("{", "{<br>&nbsp;&nbsp;&nbsp;&nbsp;");
        elem.innerHTML = elem.innerHTML.replaceAll("\n,", ",");
        elem.innerHTML = elem.innerHTML.replaceAll(",", ",<br>&nbsp;&nbsp;&nbsp;&nbsp;");
        elem.innerHTML = elem.innerHTML.replaceAll("}", "<br>}");
    });
}