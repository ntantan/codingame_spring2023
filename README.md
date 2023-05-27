# codingame_spring2023


STEPS FOR THIS CHALLENGE :
- Make a mapping for every cell to determine the distance between two points
- Make a function that generate a path with all the cells on the way with priority options
- Make a function to generate the cells that should be prioritized


-- PATH PRIORITY --

My algorithm is linking point to point, each time by chossing the closest one that is not yet linked

In addition, here is my rules when many cell could be a valide path :

- Prioritize cell that are already beaconed
- Prioritize cell that contains a ressource
- Prioritize cell with the highest number of neighboor

-- CELL PRIORITY --

What could determine the value of a cell ?

- Type of ressource
- Distance from a base, more interesting if closer
- Quantity of that ressource compared to the total on the map
- How to determine if egg or crystal is priority ?