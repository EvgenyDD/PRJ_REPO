browser.contextMenus.create({
    id: "copy-link-to-clipboard",
    title: "Download image",
    contexts: ["link"],
	targetUrlPatterns: ["*://*.furaffinity.net/view/*"]
});

function onError(error) {
  console.error(`Error: ${error}`);
}

function getSourceCode(url) 
{
	var xhr = new XMLHttpRequest();
	xhr.open('GET', url, true);
	xhr.onreadystatechange = function() 
	{
		if (xhr.readyState == 4) 
		{
			var parser = new DOMParser();
			source_code = parser.parseFromString(xhr.responseText, "text/html");    

			var doc = parser.parseFromString(xhr.responseText, "text/html");
			var links = doc.getElementsByTagName("a");
			for(var i=0, max=links.length; i<max; i++) 
			{			
				if(links[i].href.includes('d.facdn.net'))
				{					
					var path = 'http://' + links[i].href.replace('moz-extension://', '')
					path = decodeURIComponent(path)
					//console.log(path);
					console.log(path.substring(path.lastIndexOf('/') + 1));
					browser.downloads.download(
					{
						filename: path.substring(path.lastIndexOf('/') + 1),
						url: path
					});
					break;
				}
			}
		}
	};
	xhr.send();
}
	
browser.contextMenus.onClicked.addListener((info, tab) => {
    if (info.menuItemId === "copy-link-to-clipboard") {

        // Examples: text and HTML to be copied.
        const text = "This is text: " + info.linkUrl;
        // Always HTML-escape external input to avoid XSS.
        const safeUrl = info.linkUrl;
        const html = `This is HTML: <a href="${safeUrl}">${safeUrl}</a>`;	
		console.info(text);

		getSourceCode(safeUrl);
		  

        // clipboard-helper.js defines function copyToClipboard.
        const code = "copyToClipboard(" +
            JSON.stringify(text) + "," +
            JSON.stringify(html) + ");";

        browser.tabs.executeScript({
            code: "typeof copyToClipboard === 'function';",
        }).then((results) => {
            // The content script's last expression will be true if the function
            // has been defined. If this is not the case, then we need to run
            // clipboard-helper.js to define function copyToClipboard.
            if (!results || results[0] !== true) {
                return browser.tabs.executeScript(tab.id, {
                    file: "clipboard-helper.js",
                });
            }
        }).then(() => {
            return browser.tabs.executeScript(tab.id, {
                code,
            });
        }).catch((error) => {
            // This could happen if the extension is not allowed to run code in
            // the page, for example if the tab is a privileged page.
            console.error("Failed to copy text: " + error);
        });
    }
});