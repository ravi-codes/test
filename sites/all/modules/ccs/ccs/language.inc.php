<?php
//Create var for language support


function LoadLanguage($SelectedLanguage) {

	switch($SelectedLanguage)
	{
		case "nl": $Text = LoadDutch();
			break;
		case "en": $Text = LoadEnglish();
			break;
		case "at": $Text = LoadEnglish();
			break;
		case "no": $Text = LoadEnglish();
			break;
		default: $Text = LoadDutch();
			break;
	}
	return $Text;
}

function LoadDutch() {

	// General usage
	$n['OK'] = "Ok";
	$n['YES'] = "Ja";
	$n['NO'] = "Nee";
	$n['HELP'] = "Help";
	$n['HOME'] = "Home";
	$n['SETTINGS'] = "Instellingen";
	$n['LOGOUT'] = "Uitloggen";
	$n['TODAY'] = "Vandaag";
	$n['YESTERDAY'] = "Gisteren";
	$n['WEEK'] = "Week";
	$n['MONTH'] = "Maand";
	$n['SECONDS'] = "Seconden";

	//HTML Titles
	$n['LoginTitle']= "CCS Rapportages en Management";
	$n['MainTitle']= "Welkom op www.ccswebtool.com";
	$n['MainHeader'] = "CCS Rapportages en Management";
	$n['ReportTitle'] = "Het creeren van een Rapport";
	$n['ResultTitle'] = "Rapport voor";
	//Login screen
	$n['LoginButton'] = "Inloggen";
	$n['LoginWelcome'] = "Login";
	$n['LoginName'] = "Gebruikersnaam";
	$n['LoginPassword'] = "Wachtwoord";
	$n['LoginInfo'] = "
CCS webtool biedt u online rapportages over uw inkomende telefonie verkeer van
UPC Business Contact Center Services.
	
Mocht zich er een probleem voor doen dan kunt u bellen met de Customer Care Desk.
Telefoonnummer: 0800 02 12 131";

	$n['LoginError'] = "U heeft een foutieve gebruikersnaam of wachtwoord ingegeven." ;

	//Exit screen
	$n['ExitLoginError'] = "U heeft meerdere malen een verkeerde inlog poging gedaan.
Uit veiligheids overwegingen is de gebruikersnaam nu geblokeerd.

Mocht zich er een probleem voor doen dan kunt U bellen met de Customer Care Desk.
Telefoonnummer: 0800 02 12 131";

	$n["DefaultLogout"] = "U bent nu uitgelogd. Sluit nu het venster.";

        $n["MessageText"] = "De meet-gegevens in de periode 30 maart tot 2 juni 2008 hebben een te hoge 'abandonned rate' en zijn daardoor niet geheel accuraat.

Helaas zijn wij niet in staat dit te herstellen.
Wij adviseren u gebruik te maken van historische 'abandonned rate' gemiddelden in uw eigen rapportages.

Onze excuses voor het ongemak.";

	//general report
	$n['ChangeDate'] = "Wijzig de Datum";
	$n['CloseWindow'] = "Sluit Venster";
	$n['ForBNumber'] = "Voor nummer";
	$n['On'] = "op";
	$n['Report'] = "Rapport";
	$n['Type'] = "type";
	$n['DateSelect'] = "U kunt gebruik maken van een relatieve of een absolute datum. ";
	$n['SelectRelative'] = "Relatieve Datum Selectie (Vandaag; Gisteren; Verleden Week; Deze Maand; etc.)"; //"Relative Date Selection (Today, Yesterday, Month...)";
	$n['SelectAbsolute'] = "Absolute Datum Selectie (m.b.v. Kalender)"; //"Absolute Date Selection (Date Time Picker)";
	$n['RelativeDescription'] = "Voor een relative datum kunt u hieronder een selectie maken m.b.v. een datum selectie:";
	$n['RelativeDate'] = "Relatieve datum (t.o.v. Vandaag):";
	$n['Today'] = "Vandaag";
	$n['Yesterday'] = "Gisteren";
	$n['ThisWeek']= "Deze week";
	$n['LastWeek'] = "Verleden week";
	$n['ThisMonth'] = "Deze maand";
	$n['LastMonth'] = "Verleden maand";
	$n['AbsoluteDescription'] = "Voor een absolute datum kunt u hieronder een selectie maken m.b.v. de datum selectie <br> of middels het volgende format jjjj-mm-dd uu:mm<br><br>";
	$n['AbsoluteBeginDate'] = "Selecteer de begin datum/tijd: ";
	$n['AbsoluteEndDate'] = "Selecteer de eind datum/tijd: ";
	$n['ReportType'] = "Rapport naam";
	$n['SelectADate'] = "Selecteer hieronder de Start en Eind datum.";
	$n['AbsOrRel'] = "U kunt gebruik maken van een relatieve of een absolute datum.";
	$n['RadioReletive'] = "Reletieve datum selectie (Vandaag, Gisteren, maand...)";
	$n['RadioAbsolute'] = "Absolute datum selectie (Datum via een kalender selecteren)";
	$n['SelectAbsInstruction'] = "Relatieve datum: (gerekend vanaf vandaag);";
	$n['SelectStartDate'] = "Selecteer de begin datum/tijd";
	$n['SelectEndDate'] = "Selecteer de eind datum/tijd";
	$n['SelectServiceNumber'] = "Selecteer het service nummer";
	$n['CreateReportButton'] = "Maak het rapport";
	$n['ReportFor'] = "Rapport voor nummer";
	$n['StartAt'] = "Start op";
	$n['EndAt'] = "Eindigd op";
	$n['RealTimeInfo'] = "Dit rapport geeft een overzicht van alle wijzigingen in de laatste 5 min. het zal zich elke 30 seconden verversen.";
	$n['NoData'] = "Er is geen informatie beschikbaar voor dit rapport type.";
	$n['FormatDateErrorText'] = "Verkeerde datum ingave gebruik: jjjj-mm-dd uu:mm";
	$n['NoDateErrorText'] = "Er is geen datum ingegeven";
	$n['DNSelectErrorText']="U kunt maximaal 15 nummers selecteren.";

	//Report interval options
	$n['Period1Day'] = "1 dag interval";
	$n['Period1Hour'] = "1 uur interval";
      $n['Period30Min'] = "30 minuten interval";
	$n['Period5Min'] = "5 minuten interval";
	$n['Summary'] = "Opsomming per SN";

		// Main page
	$n['MainMenuHistory'] = "recente rapporten";
    $n['MainMenuInbound'] = "toegang"; //inkomend
    $n['MainMenuOutbound'] = "afleveren"; //uitgaand
	$n['MainMenuQueue'] = "wachtrij";
	$n['MainMenuSpecial'] = "speciaal";
	$n['MainMenuSupport'] = "support";

	//Description for the location and corresponding text used for reports see below
	//Report names
	$n['Report21'] = "Mobiel/Vast";  				//Menu name
	$n['Report211Header'] = "Mobiel/Vast"; 	//Line in report header
	$n['Report211HistName'] = "Mobiel\/Vast"; 		// Line used in the History menu

    $n['Report22'] = "Toegang + IVR";
    $n['Report221'] = "Inkomend";
	$n['Report221Header'] = "Inkomend verkeer + IVR Gebruik per dag.";
	$n['Report221HistName'] = "Inkomend en IVR";
	$n['Report222'] = "Inkomend";
	$n['Report222Header'] = "Inkomend verkeer + IVR Gebruik per uur.";
	$n['Report222HistName'] = "Inkomend en IVR";
	$n['Report223'] = "Inkomend";
	$n['Report223Header'] = "Inkomend verkeer + IVR Gebruik per 5 min.";
	$n['Report223HistName'] = "Inkomend en IVR";
    $n['Report224'] = "Inkomend";
    $n['Report224Header'] = "Inkomend verkeer + IVR Gebruik per 30 min.";
    $n['Report224HistName'] = "Inkomend en IVR";


	$n['Report23'] = "Herkomst (Area)";
	$n['Report23Header'] = "Herkomst (Area) verdeling";
	$n['Report23HistName'] = "Herkomst";

	$n['Report24'] = "Piek/Dal";
	$n['Report24Header'] = "Piek/Dal.";
	$n['Report24HistName'] = "Piek\/Dal";

	$n['Report25'] = "Herhaal aantallen";
	$n['Report25Header'] = "Herhaal beller overzicht.";
	$n['Report25HistName'] = "Herhaal";

	$n['Report30'] = "Afleveren";
	$n['Report31'] = "Doorschakel resultaat";
	$n['Report31Header'] = "Doorschakel resultaten per eindbestemming";
	$n['Report31HistName'] = "Doorschakel";

	$n['Report311'] = "Uitbel resultaten";
	$n['Report311Header'] = "Uitbel resultaten per dag";
	$n['Report311HistName'] = "Uitbel \/dag";

	$n['Report312'] = "Uitbel resultaten";
    $n['Report312Header'] = "Uitbel resultaten per uur";
    $n['Report312HistName'] = "Uitbel \/uur";

	$n['Report313'] = "Uitbel resultaten";
    $n['Report313Header'] = "Uitbel resultaten per 5min";
    $n['Report313HistName'] = "Uitbel \/5min";

    $n['Report314'] = "Uitbel resultaten";
    $n['Report314Header'] = "Uitbel resultaten per 30min";
    $n['Report314HistName'] = "Uitbel \/30min";

    $n['Report315'] = "Opsomming per SN";
    $n['Report315Header'] = "Opsomming per service nummer";
    $n['Report315HistName'] = "Opsomming";
    
    
    $n['Report32'] = "Totale gespreksduur per dag";
    $n['Report32Header'] = "Totale gespreksduur per dag";
    $n['Report32HistName'] = "Tot.min. per dag";

    $n['Report33'] = "Schakel resultaten";
    $n['Report33Header'] = "Schakel resultaten per geselecteerde periode";
	$n['Report33HistName'] = "Schak. Res.";



	$n['Report4'] = "Script wijzigingen";
	$n['Report4Header'] = "Script wijzigingen";
	$n['Report4HistName'] = "Script";

	$n['Report41'] = "Wachrij realtime";
	$n['Report41Header'] = "Wachrij realtime overzicht";
	$n['Report41HistName'] = "Wachrij RT";

	$n['Report42'] = "Wachtrij per dag";
	$n['Report42Header'] = "Wachtrij overzicht per dag";
	$n['Report42HistName'] = "Wachtrij per dag";

	$n['Report43'] = "Wachtrij per halfuur";
	$n['Report43Header'] = "Wachtrij per halfuur";
	$n['Report43HistName'] = "Wachtrij per halfuur";

	$n['Report51'] = "Meetpunten";
	$n['Report51Header'] = "Meetpunten rapport";
	$n['Report51HistName'] = "Meetpunten";

	$n['Report52'] = "Script wijzigingen";
	$n['Report52Header'] = "Script Wijzigingen";
	$n['Report52HistName'] = "Script";

	$n['Report53'] = "Complete uitbel data";
	$n['Report53Header'] = "Complete Uitbel Data";
	$n['Report53HistName'] = "Complete Uitbel Data";

	$n['Blockit'] = 'Plaagbellen';
	$n['Report54'] = "Toevoegingen";
	$n['Report54Header'] = "Plaagbellen/Blockit overzicht per dag";
	$n['Report54HistName'] = "Plaagbel add";

	$n['Report55'] = "Calls";
	$n['Report55Header'] = "Plaagbellen/Blockit calls per dag";
	$n['Report55HistName'] = "Plaagbel calls";

	$n['Report56'] = "OPTA Rapport";
	$n['Report56Header'] = "OPTA Rapport - Oproepen en duratie per maand";
	$n['Report56HistName'] = "OPTA";
	

	//Support options
	$n['ISEDownload'] = "Download ISE v7 software";
	$n['ManualISE'] = "Handleiding ISE";
	$n['ManualStats'] = "Handleiding CCS webtool";
	$n['Contact'] = "Stuur mail naar UPC Business";
	$n['PreviousMessages'] = "Voorgaande mededelingen";
	$n['Legal'] = "Legal warning en Disclamer";
	
	$n['Legaltext'] = "
LEGAL WARNING:
U heeft een computer benaderd welke wordt beheerd door UPC Business. Voordat u verder kunt gaan heeft u autorisatie nodig van UPC Business en u bent gebonden aan het gebruik zoals dat in de autorisatie is verwoord.
 
Ongeautoriseerd toegang of misbruik van dit systeem is verboden en wordt gezien als een delict op de van toepassing zijnde wetgeving. Indien u informatie verkregen via dit systeem zonder toestemming verspreid of openbaar maakt, zal UPC Business juridische stappen tegen u ondernemen.

DISCLAIMER: 
UPC Business is niet aansprakelijk voor enige schade geleden door Contractant als gevolg van het gebruik van CCS webtool. Contractant zal UPC Business vrijwaren voor aanspraken van derden welke zijn gebaseerd op genoemde rapportage en management applicatie.

De beschikbare rapportages zijn niet bedoeld ter vervanging van de door u te ontvangen maandelijkse factuur. Derhalve kunnen geen rechten worden ontleend aan de beschikbaar gestelde gegevens. 
	";
	

        //Audio
    $n['Audio'] = "Audio Management";
	$n['AudioServiceNumber'] = "Service nummer";
    $n['AudioType'] = "Soort Geluidsfile";
    $n['AudioName'] = "Geluidsfile Naam";
    $n['AudioSelect'] = "Selecteer file";
    $n['AudioDetails'] = "Details van geluidsfile: ";
    $n['AudioUsedIn'] = "Geluidsfile geconfigureerd voor gebruik in servicenummer: ";
    $n['AudioVFType'] = "Geluidsfile is van het type: ";
    $n['AudioVFTypeDescPlay'] = " (PLAY Speelt de inhoud van de geluidsfile af)";
    $n['AudioVFTypeDescPrompt'] = " (PROMPT Vraagt om input van de beller)";
    $n['AudioActiveFile'] = "Actieve geluidsfile: ";
    $n['AudioAlternative'] = "Alternatieve beschikbare geluidsfiles: ";
    $n['AudioFileInactive'] = "Deze geluidsfile is niet actief";
    $n['AudioFilePlay'] = "U hoort nu de inhoud van geluidsfile: ";

        $n['AudioUploadHelp1'] = "U wilt de inhoud van geluidsfile ";
        $n['AudioUploadHelp2'] = " wijzigen.<br><br>
Selecteer het nieuwe geluids fragment op uw computer.<br>
Dit fragment zal dan worden geconverteerd naar het juiste formaat met de juiste naam.<br>
De beste kwaliteit verkrijgt men door het origineel in het volgende formaat te uploaden: <b>alaw 8000Hz 16bit mono.</b><br>";

        $n['AudioVoiceFile'] = "Geluidsfile ";
        $n['AudioDeleteInfo'] = " is verwijderd/gedactiveerd.";

        $n['AudioIsActivated'] = " is geactiveerd.
Het kan echter 5 minuten duren voordat al onze systemen deze wijzigingen actief hebben gemaakt.";
        $n['AudioMadeAvailableAs'] = "De oude geluids opname is beschikbaar gemaakt als alternatieve geluidsfile met naam: ";

        $n['AudioEnterPhone'] = "Geef hier het telefoonummer waarop u gebelt wenst te worden (bv. 0101234567): ";

        $n['AudioCall'] = "Nu Bellen";
        $n['AudioPhoneHelp1'] = "Wanneer op <b><i>nu bellen</i></b> is geklikt kunt u een telefoontje verwachten op het ingegeven nummer.<br>
Volg vervolgens de insctructies op die worden gegeven door de opname applicatie<br>";

        $n['AudioBTListen'] = "Afluisteren";
        $n['AudioBTDownload'] = "Download file";
        $n['AudioBTUpload'] = "Upload nieuwe file";
        $n['AudioBTRecord'] = "Opnemen geluidsfile via telefoon";
        $n['AudioBTActivate'] = "Activeren";
        $n['AudioBTDelete'] = "Verwijderen";
        $n['AudioBTDeactivate'] = "Deactiveren van actieve geluidsfile";
        $n['AudioBTBack'] = "Terug naar file details";

        $n['WOAWReceiveCall'] = "U wordt spoedig gebelt door onze opname applicatie.";
        $n['WOAWInstructions'] = "Volg de instructies op zoals worden aangegeven in onze opname applicatie";
        $n['WOAWPhoneDestError'] = "U heeft een verkeerd telefoonnummer opgegeven probeer het opnieuw.<br> U kunt alleen geografische nummers opgeven. Gebruik het volgende formaat 010123456789<br>";


	//Feedback form
	$n['ContactInfo'] = "Stel hier uw vraag of stuur een bericht naar de support afdeling van UPC Business.<br><br>Of bel met 0800 - 0212131";
	$n['ThanksForComment'] = "Uw bericht is verzonden naar onze support afdeling. <br>Klik op \"Sluit venster\" om verder te gaan.";
	$n['Name'] = "Naam";
	$n['EmailAddress'] = "Email adres";
	$n['YourComment'] = "Geef hier uw input";
	$n['SendMessage'] = "Verstuur Bericht";

        //temp for ise7 download
        $n['DOWNLOADISE7'] = "";
        //$n['DOWNLOADISE7'] = "Let op!\n\nDeze versie van internet script editor kan pas worden gebruikt na de upgrade van \n29/30 maart 2008.\n
//Binnenkort ontvangt u meer informatie over deze upgrade.";

	$n['ReportCreationTime'] = "Rapport creatie tijd:";
	
	//Trunk Translation errors
	$n['DialedNumberErrorText']="Verkeerd data formaat gebruikt, u kunt alleen nummers gebruiken. bv. 9001234567";
    $n['RecordExistErrorText']= "Dit nummer bestaat al.";
    $n['SuccText']= "Record toegevoegd.";
    $n['NumOfDigitsErrorText']= "Het aantal cijfers moet groter zijn dan 6 en minder dan 11";
    $n['LeadZeroErrorText']="Het nummer dat u ingeeft mag geen vooloop nul bevatten. bv. 9001234567";
    
    $n['DNTErrorText']= "Wrong data format please use only numbers without leading zero.eg. 9001234567";
	return $n;
}





function LoadEnglish()
{
	// General usage
	$n['OK'] = "Ok";
	$n['YES'] = "Yes";
	$n['NO'] = "No";
	$n['HELP'] = "Help";
	$n['HOME'] = "Home";
	$n['SETTINGS'] = "Settings";
	$n['LOGOUT'] = "Logout";
	$n['TODAY'] = "Today";
	$n['YESTERDAY'] = "Yesterday";
	$n['WEEK'] = "Week";
	$n['MONTH'] = "Month";
$n['HEL'] = "Help me";
	//HTML Titles
	$n['LoginTitle']= "CCS Reporting and Management";
	$n['MainTitle']= "Welcome to www.ccswebtool.com";
	$n['MainHeader'] = "CCS Reporting and Management";
	$n['ReportTitle'] = "Create a new report";
	$n['ResultTitle'] = "Report for";
	//Login screen
	$n['LoginButton'] = "Login";
	$n['LoginWelcome'] = "Login";
	$n['LoginName'] = "Username";
	$n['LoginPassword'] = "Password";
	$n['LoginInfo'] = "
The CCS webtool offers you online reports of your incoming telephony traffic of 
UPC Business Contact Center Services.

Please enter your account details.

If you encounter a problem loggin in to the page please contact our helpdesk at
Phonenumber: +31 (0)20 778 2548";

	$n['LoginError'] = "You have entered a wrong password username combination." ;

	//Exit screen
	$n['ExitLoginError'] = "The number of loggin attempts has been exceeded.
Due to security reasons your account will be blocked.

If you encounter a problem loggin in to the page please contact our helpdesk at
Phonenumber: 0800 02 12 131";

	$n["DefaultLogout"] = "You have been loggedout please close the window.";
$n['MessageText'] = "Call data in the period 30 March up to 2 June 2008 have a high 'abandonned rate' count and as a result, are not entirely accurate.

Unfortunately we are not able to restore this data.
We advise you to use histrorical 'abandonned rate' averages in your own reports.

Our apologies for this inconvenience.
";
	//general report
	$n['ChangeDate'] = "Change the Date";
	$n['CloseWindow'] = "Close Window";
	$n['ForBNumber'] = "For number";
	$n['On'] = "on";
	$n['Report'] = "Report";
	$n['Type'] = "type";
	$n['DateSelect'] = "You can make use of  relative or absolute date selction. ";
	$n['SelectRelative'] = "Relative Date Selection (today; yesterday; this month; ...)";
	$n['SelectAbsolute'] = "Absolute Date Selection (using Calendar)";
	$n['RelativeDescription'] = "To use a relative date please select the period below:";
	$n['RelativeDate'] = "Relative date (today as reference):";
	$n['Today'] = "Today";
	$n['Yesterday'] = "Yesterday";
	$n['ThisWeek']= "This week";
	$n['LastWeek'] = "Last week";
	$n['ThisMonth'] = "This month";
	$n['LastMonth'] = "Last month";
	$n['SECONDS'] = "Seconden";
	$n['AbsoluteDescription'] = "To use a absolute date You can select the beginning and end date using the date/time pickers below<br> Or just type the date using the following format yyyy-mm-dd hh:mm<br><br>";
	$n['AbsoluteBeginDate'] = "Select the begin date/time: ";
	$n['AbsoluteEndDate'] = "Select the end date/time: ";
	$n['ReportType'] = "Report Name";
	$n['SelectADate'] = "Please select the begin and end date below.";
	$n['AbsOrRel'] = "You can make use of  relative or absolute date selction.";
	$n['RadioReletive'] = "Relative date selection (Today, Yesterday, Month...)";
	$n['RadioAbsolute'] = "Absolute date selection (Select date using a date/time picker)";
	$n['SelectAbsInstruction'] = "Relative date: (using today as reference point);";
	$n['SelectStartDate'] = "Select the begin date/time";
	$n['SelectEndDate'] = "Select the end date/time";
	$n['SelectServiceNumber'] = "Select the servicenumber";
	$n['CreateReportButton'] = "Create the report";
	$n['ReportFor'] = "Report for number";
	$n['StartAt'] = "Starts on";
	$n['EndAt'] = "Ends on";
	$n['RealTimeInfo'] = "This reports shows the chages for the last 5 minutes. The report has a automatic 30 sec. refresh.";
	$n['NoData'] = "There is no data available for this report.";
	$n['FormatDateErrorText'] = "Wrong data format please use: yyyy-mm-dd hh:mm";
	$n['NoDateErrorText'] = "Please enter a date";
	$n['DNSelectErrorText']="You can only select up to 15 numbers.";
	


	//Report interval options
	$n['Period1Day'] = "1 day interval";
	$n['Period1Hour'] = "1 hour interval";
    $n['Period30Min'] = "30 minutes interval";
	$n['Period5Min'] = "5 minutes interval";
	$n['Summary'] = "Summary";

		// Main page
	$n['MainMenuHistory'] = "recent reports";
        $n['MainMenuInbound'] = "access";
        $n['MainMenuOutbound'] = "delivery";
	$n['MainMenuQueue'] = "queue";
	$n['MainMenuSpecial'] = "special";
	$n['MainMenuSupport'] = "support";

	//Description for the location and corresponding text used for reports see below
	//Report names
	$n['Report21'] = "Mobile/Landline ";  				//Menu name
	$n['Report211Header'] = "Mobile vs. Landline segmentation"; 	//Line in report header
	$n['Report211HistName'] = "Mob vs Land"; 		// Line used in the History menu

        $n['Report22'] = "Access";
        $n['Report221'] = "Access";
        $n['Report221Header'] = "Access traffic and IVR ussage by day interval.";
        $n['Report221HistName'] = "Access and IVR";
        $n['Report222'] = "Access";
        $n['Report222Header'] = "Access traffic and IVR ussage by hourly interval.";
        $n['Report222HistName'] = "Access and IVR";
        $n['Report223'] = "Access";
        $n['Report223Header'] = "Access traffic and IVR ussage by 5 min. interval.";
        $n['Report223HistName'] = "Access and IVR";
        $n['Report224'] = "Access";
        $n['Report224Header'] = "Access traffic and IVR ussage by 30 min. interval.";
        $n['Report224HistName'] = "Access and IVR";


	$n['Report23'] = "Area route";
	$n['Report23Header'] = "Area routing";
	$n['Report23HistName'] = "Area";

	$n['Report24'] = "Peak - off peak";
	$n['Report24Header'] = "Peak vs. offpeak traffic.";
	$n['Report24HistName'] = "Peak offpeak";

	$n['Report25'] = "Number of retries";
	$n['Report25Header'] = "Number of retries.";
	$n['Report25HistName'] = "Retry";

		$n['Report30'] = "Delivery";
        $n['Report31'] = "Delivery results";
        $n['Report31Header'] = "Delivery results per destination";
        $n['Report31HistName'] = "Delivery";

        $n['Report311'] = "Delivery results";
        $n['Report311Header'] = "Delivery Results by Day";
        $n['Report311HistName'] = "Delivery \/day";

        $n['Report312'] = "Delivery results";
        $n['Report312Header'] = "Delivery results by Hour";
        $n['Report312HistName'] = "Delivery \/Hour";

        $n['Report313'] = "Delivery results";
        $n['Report313Header'] = "Delivery results by 5minday";
        $n['Report313HistName'] = "Delivery \/5min";

        $n['Report314'] = "Delivery results";
        $n['Report314Header'] = "Delivery results by 30min";
        $n['Report314HistName'] = "Delivery \/30min";

        $n['Report32'] = "Total call duration per day";
        $n['Report32Header'] = "Total call duration per day";
        $n['Report32HistName'] = "Tot.min. per day";

        $n['Report33'] = "Cleardown results";
        $n['Report33Header'] = "Total Cleardown results for the selected period";
        $n['Report33HistName'] = "Clear. Res.";

	$n['Report4'] = "Script changes";
	$n['Report4Header'] = "Script changes";
	$n['Report4HistName'] = "Script";

	$n['Report41'] = "Queue realtime";
	$n['Report41Header'] = "Queue realtime";
	$n['Report41HistName'] = "Queue RT";

	$n['Report42'] = "Queue per Day";
	$n['Report42Header'] = "Queue per Day";
	$n['Report42HistName'] = "Queue per Day";

	$n['Report43'] = "Queue per halfhour";
	$n['Report43Header'] = "Queue per halfhour";
	$n['Report43HistName'] = "Queue per 30min";

	$n['Report51'] = "Measuring points";
	$n['Report51Header'] = "Measuring points";
	$n['Report51HistName'] = "Measuring";

	$n['Report53'] = "Full call data";
	$n['Report53Header'] = "Full call data";
	$n['Report53HistName'] = "Full call data";

		$n['Blockit'] = 'Blockit';
	$n['Report54'] = "Added";
	$n['Report54Header'] = "Blockit number of blocks added by day";
	$n['Report54HistName'] = "Blockit add";

	$n['Report55'] = "Calls";
	$n['Report55Header'] = "Blockit number of blocked calls by day";
	$n['Report55HistName'] = "Blockit calls";
	
	$n['Report56'] = "OPTA Report";
	$n['Report56Header'] = "OPTA Rapport - Calls and duration by month";
	$n['Report56HistName'] = "OPTA";
	
	$n['Report52'] = "Script changes";
	$n['Report52Header'] = "Script changes";
	$n['Report52HistName'] = "Script";
	
	$n['Report315'] = "Summary per SN";
    $n['Report315Header'] = "Summary per service number";
    $n['Report315HistName'] = "Summary";
    
    
	//Support options
	$n['ISEDownload'] = "Download ISE v7 software";
	$n['ManualISE'] = "Manual ISE";
	$n['ManualStats'] = "Manual CCS webtool";
	$n['Contact'] = "Mail UPC Business";
	$n['PreviousMessages'] = "Previous messages";
	$n['Legal'] = "Legal warning and Disclamer";
	
	$n['Legaltext'] = "
LEGAL WARNING:
You have accessed a computer managed by UPC Business. You are required to have authorization from UPC Business before you proceed and you are strictly limited to the use set out within that authorization.

Unauthorized access to or misuse of this system is prohibited and constitutes an offence to the applicable legislation. If you disclose any information obtained through this system without authority UPC Business may take legal action against you.

DISCLAIMER: 
UPC Business is not responsible for any damage that is incurred to the contracting party as a result of using CCS webtool. Contracting party relieves UPC Business of any third-party claims that are based on said reporting and management tool. 

The available reports are not intended to replace the monthly invoice that you receive. Therefore, no rights can be derived from the information made available. 
	";

        //Audio
	$n['Audio'] = "Audio Management";
    $n['AudioServiceNumber'] = "Servicenumber";
    $n['AudioType'] = "Type of voicefile";
    $n['AudioName'] = "Voicefile Name";
    $n['AudioSelect'] = "Select a file";
    $n['AudioDetails'] = "Details of voicefile: ";
    $n['AudioUsedIn'] = "The Voicefile has been configured for servicenumber: ";
    $n['AudioVFType'] = "Voicefile is of the type: ";
    $n['AudioVFTypeDescPlay'] = " (PLAY Echo's the content of the voicefile.)";
    $n['AudioVFTypeDescPrompt'] = " (PROMPT Asks input from the user.)";
    $n['AudioActiveFile'] = "Active voicefile: ";
    $n['AudioAlternative'] = "Alternative voicefiles: ";
    $n['AudioFileInactive'] = "This voicefile is inactive";
    $n['AudioFilePlay'] = "You are listening to the content of voicefile: ";

    $n['AudioUploadHelp1'] = "You wish to change the content of voicefile: ";
    $n['AudioUploadHelp2'] = " <br><br>
Select the new voicefile on your computer using the windows file selectiontool.<br>
This file will be automatically converted to the appropriate format and filename.<br>
The best quality is obtained by using the following audio format: <b>alaw 8000Hz 16bit mono.</b><br>";

        $n['AudioVoiceFile'] = "Voicefile ";
        $n['AudioDeleteInfo'] = " has been deactivated/deleted.";

        $n['AudioIsActivated'] = " has been activated";
        $n['AudioMadeAvailableAs'] = "The old voicfile has been made available as an alternative voicefiles. Using filename: ";

        $n['AudioEnterPhone'] = "Please enter the phonenumber on which you want to receive the call (eg. 0101234567): ";

        $n['AudioCall'] = "Call me now";
        $n['AudioPhoneHelp1'] = "After clicking <b><i>Call me now</i></b> you can expect a phonecall momentarily.<br>
Please follow the instructions as given by our recording application.<br>";

        $n['AudioBTListen'] = "Play back";
        $n['AudioBTDownload'] = "Download file";
        $n['AudioBTUpload'] = "Upload new file";
        $n['AudioBTRecord'] = "Record file by phone";
        $n['AudioBTActivate'] = "Activate";
        $n['AudioBTDelete'] = "Delete";
        $n['AudioBTDeactivate'] = "Deactivate active voicefile";
        $n['AudioBTBack'] = "Back to file details";

        $n['WOAWReceiveCall'] = "You can expect a phonecall by our voicefile recording application momentarily.";
        $n['WOAWInstructions'] = "Please follow the instructions as given by our recording application.";
        $n['WOAWPhoneDestError'] = "You have provided a wrong phonenumber format please try again using the following format: 010123456789<br>";

	//Feedback form
	$n['ContactInfo'] = "Send a message to the support team.";
	$n['ThanksForComment'] = "Thanks for your input. <br>Click close window to continue.";
	$n['Name'] = "Name";
	$n['EmailAddress'] = "Email address";
	$n['YourComment'] = "Enter your message";
	$n['SendMessage'] = "Send message";
	
	//Trunk Translation errors
    $n['DialedNumberErrorText'] = "Wrong data format please use only numbers without leading zero.eg. 9001234567";
    $n['RecordExistErrorText']= "Dialed number Already Exist";
    $n['SuccText']= "Record Added";
    $n['NumOfDigitsErrorText']= "Number of digits should be greater than 6 and less than 11";
    $n['LeadZeroErrorText']="Please use numbers without Leading zero. eg. 9001234567";
    
    //Trunk Translation Delete Errors
    $n['DeleteSuccText']="Record Deleted";
    $n['DNNotExistText']="Record Does Not exist";
    $n['DNTErrorText']= "Wrong data format please use only numbers without leading zero.eg. 9001234567";
    
    $n['EmailChange'] = "Change EmailID";
    $n['VmailBTBack'] = "Back to Customer details";
    $n['AudioUploadHelp3'] = "You wish to upload new voicefile";
    $n['AudioUploadHelp4'] = " <br><br>
Select the new voicefile on your computer using the windows file selection tool.<br>
This file will be automatically converted to the appropriate format.<br>
The best quality is obtained by using the following audio format: <b>alaw 8000Hz 16bit mono.</b><br>";
	//temp for ise7 doanwload
        //$n['DOWNLOADISE7'] = "";
        //$n['DOWNLOADISE7'] = "Caution!\n\nThis version of internet script editor can only be used after the upgrade of \n29/30 March 2008.\n";
	$n['ReportCreationTime'] = "Rapport creatie tijd:";
	return $n;
}

function LoadGerman()
{
	$n['OK'] = "Ok";
	$n["YES"] = "Yes";
	$n["NO"] = "No";
	$n["HELP"] = "Help";
	$n['LoginButton'] = "Login";
	$n['LoginWelcome'] = "AT Login Welkom";
	$n['LoginName'] = "AT Login Gebruikersnaam";
	$n['LoginPassword'] = "AT Login Password";
	$n['LoginInfo'] = "";
	$n['LoginError'] = "AT You have entered a invalid username password combination.";
	$n['ExitLoginError'] = "AT Login exit after # attemps";
	return $n;
}

function LoadNorwegian()
{
	$n['OK'] = "Ok";
	$n["YES"] = "Yes";
	$n["NO"] = "No";
	$n["HELP"] = "Help";
	$n['LoginButton'] = "Login";
	$n['LoginWelcome'] = "NO Login Welkom";
	$n['LoginName'] = "NO Login Gebruikersnaam";
	$n['LoginPassword'] = "NO Login Password";
	$n['LoginInfo'] = "";
	$n['LoginError'] = "NO You have entered a wrong username password combination.";
	$n['ExitLoginError'] = "NO Login exit after # attemps";
	return $n;
}
?>
