
SetHeader("Content-Type", "text/html")

out = "<h1>Test Page</h1>" ..
    "<ul>"

for i = 1,15 do
    out = out .. "<li>Test " .. i .. "</li>"
end

out = out ..
    "</ul>";

Output(out)
