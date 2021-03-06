<!DOCTYPE html>
<html lang="en"><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="description" content="">
    <meta name="author" content="">
    <link rel="shortcut icon" href="img/favicon.ico">

    <title>btctrackr.com</title>

    <!-- Bootstrap core CSS -->
    <link href="./bootstrap/css/bootstrap.min.css" rel="stylesheet">
    <link href="./bootstrap/css/bootstrap.css" rel="stylesheet">

    <!-- Custom styles for this template -->
    <link href="http://getbootstrap.com/examples/cover/cover.css" rel="stylesheet">

<!-- Custom css
 -->
    <link href="./css/main.css" rel="stylesheet">

  <body>


    <div class="site-wrapper">

      <div class="site-wrapper-inner">

        <div class="cover-container">
         
          <div id="main">

            <div id="trackdiv">
              <img src="./img/logo_text_orange.png"><br />
              <!--<h1 class="cover-heading" style="color:white">Enter an address:</h1>-->
              <span style="font-size:18px;"> Enter an address to find out which other addresses likely belong to the same person. </span>
              <input id="go-input" style="margin-top:3px; background: rgba(255,255,255, 0.4); outline: none; padding-left: 12px; width:520px" name="address" value="" placeholder="3J98t1WpEZ73CNmQviecrnyiWrnqRhWNLy" autocomplete="off" required>
              <button type="button" id="go-button" style="margin-top: 0px; height: 67px; width: 125px; vertical-align: bottom; font-size: 30px; font-weight: lighter; font-family: Lato-Hairline;" class="btn btn-large btn-warning">Track</button>
            </div>

            <div id="pathdiv" style="display:none">
               <img src="./img/logo_text_orange.png"><br />
              <!--<h1 class="cover-heading" style="color:white">Enter an address:</h1>-->
              <span style="font-size:18px;"> Enter two addresses to find their shortest connecting path. </span>

              <input id="go-input" style="margin-top:3px; margin-bottom:3px; background: rgba(255,255,255, 0.4); outline: none; padding-left: 12px; width:520px" name="address" value="" placeholder="3J98t1WpEZ73CNmQviecrnyiWrnqRhWNLy" autocomplete="off" required>
              <br />
              <img src="./img/down_arrow.png" style="width:33px; height:40px"> &nbsp; &nbsp;                        
              <img src="./img/down_arrow.png" style="width:33px; height:40px"> &nbsp; &nbsp;
              <img src="./img/down_arrow.png" style="width:33px; height:40px">
              <br />
              <input id="go-input" style="margin-top:3px; background: rgba(255,255,255, 0.4); outline: none; padding-left: 12px; width:520px" name="address" value="" placeholder="1dice48cexAaxtGiTRJTNUT1mBqqunzM1" autocomplete="off" required>
                            
              <br /><br />
              <button type="button" id="go-button" style="margin-top: 0px; height: 67px; width: 200px; vertical-align: bottom; font-size: 30px; font-weight: lighter; font-family: Lato-Hairline;" class="btn btn-large btn-warning">Shortest Path</button>
            </div>

            <div class="mastfoot">
              <div class="inner">
                <a id="trackclick" href="#">Track</a> &middot;
                <a id="pathclick" href="#">Shortest Path</a> &middot;
                <!--<a id="contactclick" href="#">Contact</a>-->
                <a id="contactclick" href="#" rel="popover" data-placement="top" data-content="admin@btctrackr.com">Contact</a>
              </div>
            </div>

          </div>
        </div>

      </div>

    </div>

    <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
    <script src="bootstrap/js/jquery.min.js"></script>
    <script src="bootstrap/js/bootstrap.min.js"></script>
    <script src="bootstrap/js/docs.min.js"></script>

    <script>

    $(document).ready(function(){
      $('#main').css("display", "none");
      $('#main').fadeIn(1000);
      $('#contactclick').popover({placement:'top'});
    });

    $('#trackclick').click(function() {
      $('#pathdiv').css("display", "none");
      $('#trackdiv').fadeIn(1000);
    });

    $('#pathclick').click(function() {
      $('#trackdiv').css("display", "none");
      $('#pathdiv').fadeIn(1000);
    });

    </script>


</body>

</html>