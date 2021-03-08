(function() {
  /* Set the active page */
  var setPage = function(pgname) {
    /* Clear all pages */
    var all_pages = document.querySelector("main").querySelectorAll("div");
    for (var i=0; i<all_pages.length; i++) {
      all_pages[i].style["display"] = "none";
    }
    document.querySelector("div#"+pgname).style["display"] = "block";
  };

  var bindPage = function(what, pgname) {
    document.querySelector(what).addEventListener("click", function() {
      setPage(pgname);
      history.pushState({}, "", "/"+pgname);
    });
  };

  var upload = async function() {
    var text = document.querySelector("#upload_text").value;
    var body = new FormData();
    body.append('paste', text);
    var resp = await fetch('/upload', {
      method: 'POST',
      body: body,
    });
    var data = await resp.json();
    console.log(data);
    loadPaste(data['name'], data['key']);
    history.pushState({}, '', '/'+data['name']+'/'+data['key']);
  };

  var loadPaste = async function(fileId, keyData) {
    var reqURI = '/load?file=' + encodeURIComponent(fileId);
    reqURI += '&key=' + encodeURIComponent(keyData);
    var resp = await fetch(reqURI);
    // TODO: error handling
    var text = await resp.text();
    console.log("Body: " + text);
    document.querySelector("#load_text").value = text;
    setPage("load");
  };

  var maybeLoadFromURL = async function() {
    var path = document.location.pathname;
    if (path.length == 0) {
      return;
    }
    if (path[0] == "/") {
      path = path.slice(1);
    }
    var slicePos = path.indexOf("/");
    if (slicePos == -1) {
      return;
    }
    var fileId = path.slice(0, slicePos);
    var keyData = path.slice(slicePos+1);
    console.log("File ID: " + fileId);
    console.log("Key Data: " + keyData);
    await loadPaste(fileId, keyData);
  };

  bindPage("#home-link", "main");
  bindPage("#upload-link", "upload");
  document.querySelector("#btn-upload").addEventListener("click", upload);
  maybeLoadFromURL();
}());
