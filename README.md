# vehicle-delivey-route-design-and-simulation
Location-Routing problem

The project include codes we have written for 2019 O.R. & Analytics Student Team Competition. The problem of General Motors involved designing the vehicle delivery network (where to locate warehouses, and what is the capacity) and the routing from plant to the final warehouse for each plant-dealer-vehicle combination. The objective was to minimize total cost (both shipping and yard operations), while satisfying the business rules, such as meeting delivery schedules. We preprocessed 5 millions rows of delivery data using R. We determined the delivery routes by a Mixed-Integer optimization model written in GAMS & C++ and solved with GUROBI. Then, we simulated the delivery of vehicles using C++ via discrete-event methodology.

Note: Due to confidentiality, datasets and R codes are not uploaded.
     

