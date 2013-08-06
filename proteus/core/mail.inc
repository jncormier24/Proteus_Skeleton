<?php
class systemEmail extends proteusEmail
{
	public function __construct($toEmail, $subject, $body)
	{
		parent::__construct($toEmail, $subject, $body);
	}
	public function send($verbose=true)
	{		
		$conf = new siteConfig();
		
		$p = new fileParser("themes/".$config["frontend_theme"]."/emails/".$conf->defaultEmailTemplate);		
		$p->pushParam("content", $this->body);		
		
		parent::send($verbose, true, $p);
	}
}
class proteusEmail
{
	public $toAddress;
	public $fromAddress;
	public $fromName = '';
	public $subject;
	public $body;
	public $bcc;

	public function __construct($toEmail, $subject = '', $body = '', $fromAddress = '')
	{
		global $config;
		
		$conf = new siteConfig();
		
		$this->fromAddress = (!$fromAddress ? $conf->sitemail : $fromAddress);
		
		$this->toAddress = $toEmail;
		$this->subject = $subject;
		$this->body = $body;		
	}
	public function send($verbose=false, $isHtml=true, $parser=null)
	{
		global $config;		
		
		$conf = new siteConfig();		
		
		if ($parser)
		{
			// Push default allowed config parameters for email templates
			$parser->pushParam("businessName", $conf->businessName);
			$parser->pushParam("businessAddr", $conf->businessAddr);
		}
		
		$mail = new PHPMailer();
		$mail->IsSendmail();
		$mail->IsHTML($isHtml);
		$mail->SetFrom($this->fromAddress, $conf->businessName." Mailer");
		$mail->AddReplyTo($this->fromName, $conf->businessName." Mailer");

		$mail->Subject = $this->subject;
		$mail->MsgHTML($parser ? $parser->render() : $this->body, $config["scripturl"]);
		
		// TODO: Add SMTP support down the road (7/13)
		
		/*$mail->IsSMTP();                                      // set mailer to use SMTP
		$mail->Host = $config[smtp_host];  // specify main and backup server
		$mail->SMTPAuth = true;     // turn on SMTP authentication
		$mail->Username = $config[smtp_user];  // SMTP username
		$mail->Password = $config[smtp_pass]; // SMTP password*/		
		
		//Add recipient
		$mail->AddAddress($this->toAddress);

		if ($this->bcc)
		{
			$bccList = split(";", $this->bcc);
			foreach($bccList as $addr)
			{
				$mail->AddBCC($addr);	
			}
		}
		
		if(!@$mail->Send())
		{
			//error_log("Error sending email: $mail->ErrorInfo");
			if ($verbose) throw new Exception("Error sending email. Detail: ".$mail->ErrorInfo);
		}	
	}
}
?>