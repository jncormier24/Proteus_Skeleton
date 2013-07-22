function showSubscriptionManage(subID)
{
	showSubscribe(0, subID).then(function()
	{
		$("#subscribeWin").on("dialogbeforeclose", function() 
		{
			$.showLoading("Please Wait");
			
			window.location='index'; 
			return false; 
		});
	});
}