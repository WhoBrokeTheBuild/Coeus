
SetHeader("Content-Type", "text/html")
SetHeader("X-Hi-Tim", "nope")

out = "<h1>Test Page</h1>" ..
    "<ul>"

for i = 0,10 do
    out = out .. "<li>Test " .. i .. "</li>"
end

out = out ..
    "</ul>";

Output(out)
