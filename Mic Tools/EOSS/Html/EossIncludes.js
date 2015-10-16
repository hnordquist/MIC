// EossReader Dispatch Interfaces
try
{
var IAgent = new ActiveXObject("BrowserAgent.WebBrowserAgent"); 
var ISession = IAgent.Session;
var ISeal = ISession.Seal;
var IGroup = ISession.Group;
var IDocument = ISession.Document;
}
catch (e)
{
	window.location.href ="EossReaderBlank.htm";
}
