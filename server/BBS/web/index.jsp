<%@page contentType="text/html" pageEncoding="UTF-8"%>
<!DOCTYPE html>
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <meta http-equiv="refresh" content="5; URL=http://192.168.43.34:8080/BBS/Home">
        <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">

        <title>Big Brother Sec</title>
        <% boolean isActive = (boolean)application.getAttribute("status"); %>
        <% boolean isMoving = (boolean)application.getAttribute("movement"); %>
    </head>
    <body>
        <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p" crossorigin="anonymous"></script>
    <center>
        <div class="card">
  <h5 class="card-header">Big Brother Sec</h5>
  <div class="card-body">
    

        <br>
        <p class="text-center" id="alarm">
            <% 
            if(isMoving)
                out.println("Movement Detected!");
            %>
        </p>
        <p class="text-center" id="status">
            <%
                String status = "Disarmed";
                if(isActive)
                    status = "Armed";
                out.println("Status: "+status);
            %>
        </p>
        <a class="btn btn-primary btn-lg" href="Activate">
            Activate
        </a>
        <a class="btn btn-primary btn-lg" href="Deactivate">
            Deactivate
        </a>
      </div>
</div>
    </center>
    </body>
</html>
