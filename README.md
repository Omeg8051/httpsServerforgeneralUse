# httpsServerforgeneralUse
This is an rushed attempt to make something somewhat useable.

<h3>
How to get it to work?
</h3>

<p>before we start</p>
<h1>!!DO NOT PUBLISH THE KEY AND CHAIN FILES TO GIT HUB!!</h1>
<h2>Put the key and chain somethere else. And specify it in the parameter.</h2>

<p>Ok, with that out of the way. let's begin.</p>

  First, create a folder in the root called obj.
  <br>
  <br>
  Second, Get 2 files from Let's Encrypt cert bot.
  <br>
  There will be a `private key` and a `chain file`
  <br>
  <br>
  lunch server with
  ```
  ./server_vid.out -p "your port here" -k "your private key.pem" -c "your full chain file"
  ```
  <br>

  you can always add `-h` to the command for the built-in manual.
  
  
