$(function() {
  $('#status').hide();

  function showError(msg) {
    $('#status').empty().text(msg).show();
    
  }

  $('#convert').click(function(event) {
    event.preventDefault();
    $('#status').hide();

    var fileInput = $(':file')[0];
    if(fileInput.files.length < 1) {
      showError("No file selected");
      return;
    }

    if(fileInput.files[0].size > 256*1024) {
      showError("Refusing to send more than 256k.");
      return;
    }

    var reader = new FileReader();
    reader.onload = function() {
      console.log(reader.result);
      $.ajax({
        url: '/convert',
        beforeSend: function(xhr) {
          xhr.setRequestHeader("Content-type", "application/json");
        },
        type: 'POST',
        cache: false,
        data: JSON.stringify({
          data:                   btoa(reader.result),
          ignoreValidationErrors: $('#ignore-errors').prop('checked'),
          loop:                   $('#loop-song').prop('checked'),
          compress:               $('#compress-exe').prop('checked')
        })
      }).done(function(data) {
        window.location = data.result;
      }).fail(function(data) {
        if(data.responseJSON && data.responseJSON.error) {
          showError(data.responseJSON.error);
        } else {
          if(data.status == 413) {
            showError("Transmitted file to big. How can that be if you're using this frontend?");
          } else {
            showError("Error (" + data.status + "). Sorry, probably not your fault. Please tell flo at base13 dot de.");
          }
        }
      });
    };

    reader.readAsBinaryString(fileInput.files[0]);
  });

});
