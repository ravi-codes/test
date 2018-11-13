<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">

<style>
/* Client-specific Styles */
    #outlook a{padding:0;} /* Force Outlook to provide a "view in browser" button. */
    body{width:100% !important;} .ReadMsgBody{width:100%;} .ExternalClass{width:100%;} /* Force Hotmail to display emails at full width */
    body{-webkit-text-size-adjust:none;} /* Prevent Webkit platforms from changing default text sizes. */

    /* Reset Styles */
    body{margin:0; padding:0;}
    img{border:0; height:auto; line-height:100%; outline:none; text-decoration:none;}
    table td{border-collapse:collapse;}
    #backgroundTable{height:100% !important; margin:0; padding:0; width:100% !important;}

    /* Template Styles */

    /* /\/\/\/\/\/\/\/\/\/\ STANDARD STYLING: COMMON PAGE ELEMENTS /\/\/\/\/\/\/\/\/\/\ */

    /**
    * @tab Page
    * @section background color
    * @tip Set the background color for your email. You may want to choose one that matches your company's branding.
    * @theme page
    */
    body, #backgroundTable{
       background-color:#FAFAFA;
    }

    /**
    * @tab Page
    * @section email border
    * @tip Set the border for your email.
    */
    #templateContainer{
       border: 1px solid #DDDDDD;
    }

    /**
    * @tab Page
    * @section heading 1
    * @tip Set the styling for all first-level headings in your emails. These should be the largest of your headings.
    * @style heading 1
    */
    h1, .h1{
      color:#F48C00;
      display:block;
      font-family:Arial;
      font-size:34px;
      font-weight:bold;
      line-height:100%;
      margin-top:0;
      margin-right:0;
      margin-bottom:10px;
      margin-left:0;
       text-align:left;
    }

    /**
    * @tab Page
    * @section heading 2
    * @tip Set the styling for all second-level headings in your emails.
    * @style heading 2
    */
    h2, .h2{
       color:#F48C00;
      display:block;
       font-family:Arial;
       font-size:30px;
       font-weight:bold;
       line-height:100%;
      margin-top:0;
      margin-right:0;
      margin-bottom:10px;
      margin-left:0;
       text-align:left;
    }

    /**
    * @tab Page
    * @section heading 3
    * @tip Set the styling for all third-level headings in your emails.
    * @style heading 3
    */
    h3, .h3{
       color:#202020;
      display:block;
       font-family:Arial;
       font-size:26px;
       font-weight:bold;
       line-height:100%;
      margin-top:0;
      margin-right:0;
      margin-bottom:10px;
      margin-left:0;
       text-align:left;
    }

    /**
    * @tab Page
    * @section heading 4
    * @tip Set the styling for all fourth-level headings in your emails. These should be the smallest of your headings.
    * @style heading 4
    */
    h4, .h4{
       color:#202020;
      display:block;
       font-family:Arial;
       font-size:22px;
       font-weight:bold;
       line-height:100%;
      margin-top:0;
      margin-right:0;
      margin-bottom:10px;
      margin-left:0;
       text-align:left;
    }

    /* /\/\/\/\/\/\/\/\/\/\ STANDARD STYLING: PREHEADER /\/\/\/\/\/\/\/\/\/\ */

    /**
    * @tab Header
    * @section preheader style
    * @tip Set the background color for your email's preheader area.
    * @theme page
    */
    #templatePreheader{
       background-color:#FAFAFA;
    }

    /**
    * @tab Header
    * @section preheader text
    * @tip Set the styling for your email's preheader text. Choose a size and color that is easy to read.
    */
    .preheaderContent div{
       color:#505050;
       font-family:Arial;
       font-size:10px;
       line-height:100%;
       text-align:left;
    }

    /**
    * @tab Header
    * @section preheader link
    * @tip Set the styling for your email's preheader links. Choose a color that helps them stand out from your text.
    */
    .preheaderContent div a:link, .preheaderContent div a:visited, /* Yahoo! Mail Override */ .preheaderContent div a .yshortcuts /* Yahoo! Mail Override */{
       color:#336699;
       font-weight:normal;
       text-decoration:underline;
    }

    /* /\/\/\/\/\/\/\/\/\/\ STANDARD STYLING: HEADER /\/\/\/\/\/\/\/\/\/\ */

    /**
    * @tab Header
    * @section header style
    * @tip Set the background color and border for your email's header area.
    * @theme header
    */
    #templateHeader{
       background-color:#FFFFFF;
       border-bottom:0;
    }

    /**
    * @tab Header
    * @section header text
    * @tip Set the styling for your email's header text. Choose a size and color that is easy to read.
    */
    .headerContent{
       color:#202020;
       font-family:Arial;
       font-size:10px;
       font-weight:bold;
       line-height:100%;
       padding:0;
       text-align:left;
       vertical-align:middle;
    }


    /**
    * @tab Header
    * @section header link
    * @tip Set the styling for your email's header links. Choose a color that helps them stand out from your text.
    */
    .headerContent a:link, .headerContent a:visited, /* Yahoo! Mail Override */ .headerContent a .yshortcuts /* Yahoo! Mail Override */{
       color:#336699;
       font-weight:normal;
       text-decoration:underline;
    }

    #headerImage{
      height:auto;
      max-width:600px !important;
    }

    /* /\/\/\/\/\/\/\/\/\/\ STANDARD STYLING: MAIN BODY /\/\/\/\/\/\/\/\/\/\ */

    /**
    * @tab Body
    * @section body style
    * @tip Set the background color for your email's body area.
    */
    #templateContainer, .bodyContent{
       background-color:#FFFFFF;
    }

    /**
    * @tab Body
    * @section body text
    * @tip Set the styling for your email's main content text. Choose a size and color that is easy to read.
    * @theme main
    */
    .bodyContent div{
       color:#262727;
       line-height: 13.5pt;
       text-align:left;
       font-size: 9.0pt;
       font-family: "Verdana","sans-serif";
    }
    
    .subjectContent div {
        font-size: 12.0pt;
			  line-height: 120%;
			  font-weight: normal;
			  margin-bottom: 10px;
		    font-family:"Verdana","sans-serif";
        color:#F48C00
		  } 

    /**
    * @tab Body
    * @section body link
    * @tip Set the styling for your email's main content links. Choose a color that helps them stand out from your text.
    */
    .bodyContent div a:link, .bodyContent div a:visited, /* Yahoo! Mail Override */ .bodyContent div a .yshortcuts /* Yahoo! Mail Override */{
       color:#336699;
       font-weight:normal;
       text-decoration:underline;
    }

    .bodyContent img{
      display:inline;
      height:auto;
    }

    /* /\/\/\/\/\/\/\/\/\/\ STANDARD STYLING: FOOTER /\/\/\/\/\/\/\/\/\/\ */

    /**
    * @tab Footer
    * @section footer style
    * @tip Set the background color and top border for your email's footer area.
    * @theme footer
    */
    #templateFooter{
       background-color:#FFFFFF;
       border-top:0;
    }

    /**
    * @tab Footer
    * @section footer text
    * @tip Set the styling for your email's footer text. Choose a size and color that is easy to read.
    * @theme footer
    */
    .footerContent div{
       color:#707070;
       font-family:Arial;
       font-size:12px;
       line-height:125%;
       text-align:left;
    }

    /**
    * @tab Footer
    * @section footer link
    * @tip Set the styling for your email's footer links. Choose a color that helps them stand out from your text.
    */
    .footerContent div a:link, .footerContent div a:visited, /* Yahoo! Mail Override */ .footerContent div a .yshortcuts /* Yahoo! Mail Override */{
       color:#336699;
       font-weight:normal;
       text-decoration:underline;
    }

    .footerContent img{
      display:inline;
    }

    /**
    * @tab Footer
    * @section social bar style
    * @tip Set the background color and border for your email's footer social bar.
    * @theme footer
    */
    #social{
       background-color:#FAFAFA;
       border:0;
    }

    /**
    * @tab Footer
    * @section social bar style
    * @tip Set the background color and border for your email's footer social bar.
    */
    #social div{
       text-align:center;
    }

    /**
    * @tab Footer
    * @section utility bar style
    * @tip Set the background color and border for your email's footer utility bar.
    * @theme footer
    */
    #utility{
       background-color:#FFFFFF;
       border:0;
    }

    /**
    * @tab Footer
    * @section utility bar style
    * @tip Set the background color and border for your email's footer utility bar.
    */
    #utility div{
       text-align:center;
    }
</style>
  
  <title><?php print $title; ?></title>

</head>
<body leftmargin="0" marginwidth="0" topmargin="0" marginheight="0" offset="0" style="-webkit-text-size-adjust: none;margin: 0;padding: 0;background-color: #FAFAFA;width: 100% !important;">
<center>
    <table border="0" cellpadding="0" cellspacing="0" height="100%" width="100%" id="backgroundTable" style="margin: 0;padding: 0;background-color: #FAFAFA;height: 100% !important;width: 100% !important;">
      <tr>
        <td align="center" valign="top" style="border-collapse: collapse;">
          <!-- // Begin Template Preheader \\ -->
          <table border="0" cellpadding="10" cellspacing="0" width="600" id="templatePreheader" style="background-color: #FAFAFA;">
            <tr>
              <td valign="top" class="preheaderContent" style="border-collapse: collapse;">

                <!-- // Begin Module: Standard Preheader \ -->
                <table border="0" cellpadding="10" cellspacing="0" width="100%">
                  <tr>
                    <td valign="top" style="border-collapse: collapse;">
                      <div mc:edit="std_preheader_content" style="color: #505050;font-family: Arial;font-size: 10px;line-height: 100%;text-align: left;">

                      </div>
                    </td>
                    <td valign="top" style="border-collapse: collapse;">
                      <div mc:edit="std_preheader_content" style="color: #505050;font-family: Arial;font-size: 10px;line-height: 100%;text-align: left;">
                        
                      </div>
                    </td>
                  </tr>
                </table>
                <!-- // End Module: Standard Preheader \ -->

              </td>
            </tr>
          </table>
          <!-- // End Template Preheader \\ -->
          <table border="0" cellpadding="0" cellspacing="0" width="600" id="templateContainer" style="border: 1px solid #DDDDDD;background-color: #FFFFFF;">
            <tr>
              <td align="center" valign="top" style="border-collapse: collapse;">
                <!-- // Begin Template Header \\ -->
                <table border="0" cellpadding="0" cellspacing="0" width="600" id="templateHeader" style="background-color: #FFFFFF;border-bottom: 0;">
                  <tr>
                    <td class="headerContent" style="border-collapse: collapse;color: #202020;font-family: Arial;font-size: 10px;font-weight: bold;line-height: 100%;padding: 0;text-align: left;vertical-align: middle;">

                      <!-- // Begin Module: Standard Header Image \\ -->
                      <img src="<?php print $product_logo;?>" style="width: 150px;border: 0;height: auto;line-height: 100%;outline: none;text-decoration: none;" id="headerImage campaign-icon">
                      <!-- // End Module: Standard Header Image \\ -->

                    </td>
                  </tr>
                </table>
                <!-- // End Template Header \\ -->
              </td>
            </tr>
            <tr>
              <td align="center" valign="top" style="border-collapse: collapse;">
                <!-- // Begin Template Body \\ -->
                <table border="0" cellpadding="0" cellspacing="0" width="600" id="templateSubject">
                  <tr>
                    <td valign="top" class="subjectContent" style="border-collapse: collapse;">
                      <table border="0" cellpadding="20" cellspacing="0" width="100%">
                        <tr>
                          <td valign="top" style="border-collapse: collapse;">
                            <div style="font-size: 12.0pt;line-height: 120%;font-weight: normal;margin-bottom: 10px;font-family: &quot;Verdana&quot;,&quot;sans-serif&quot;;color: #F48C00;">
                              <?php echo $subject; ?>
                            </div>
                          </td>
                        </tr>
                      </table>
                    </td>
                  </tr>
                </table>
                <!-- // End Template Body \\ -->
              </td>
            </tr>
            <tr>
              <td align="center" valign="top" style="border-collapse: collapse;">
                <!-- // Begin Template Body \\ -->
                <table border="0" cellpadding="0" cellspacing="0" width="600" id="templateBody">
                  <tr>
                    <td valign="top" class="bodyContent" style="border-collapse: collapse;background-color: #FFFFFF;">

                      <!-- // Begin Module: Standard Content \\ -->
                      <table border="0" cellpadding="20" cellspacing="0" width="100%">
                        <tr>
                          <td valign="top" style="border-collapse: collapse;">
                            <div style="color: #262727;line-height: 13.5pt;text-align: left;font-size: 9.0pt;font-family: &quot;Verdana&quot;,&quot;sans-serif&quot;;">
                              <?php echo $body; ?>
                            </div>
                          </td>
                        </tr>
                      </table>
                      <!-- // End Module: Standard Content \\ -->

                    </td>
                  </tr>
                </table>
                <!-- // End Template Body \\ -->
              </td>
            </tr>
            <tr>
              <td align="center" valign="top" style="border-collapse: collapse;">
                <!-- // Begin Template Footer \\ -->
                <table border="0" cellpadding="10" cellspacing="0" width="600" id="templateFooter" style="background-color: #FFFFFF;border-top: 0;">
                  <tr>
                    <td valign="top" class="footerContent" style="border-collapse: collapse;">

                      <!-- // Begin Module: Standard Footer \\ -->
                      <table border="0" cellpadding="10" cellspacing="0" width="100%">
                        <tr>
                          <td valign="top" width="350" style="border-collapse: collapse;">
                            <div mc:edit="std_footer" style="color: #707070;font-family: Arial;font-size: 12px;line-height: 125%;text-align: left;">
                              <?php print $footer;?>
                            </div>
                          </td>
                          <td valign="top" width="190" style="border-collapse: collapse;">

                          </td>
                        </tr>
                      </table>
                      <!-- // End Module: Standard Footer \\ -->

                    </td>
                  </tr>
                </table>
                <!-- // End Template Footer \\ -->
              </td>
            </tr>
          </table>
          <br>
        </td>
      </tr>
    </table>
  </center>


</body>
</html>
