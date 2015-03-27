var fs = require('fs');

module.exports = {
  'fixture': function(file) {
    var fixture = fs.readFileSync(__dirname + '/../fixtures/' + file, 'utf8');
    return fixture;
  }
};

