# Tests

`test_v112.c` checks 3000 randomly generated train questions across three levels, review priority and V9/V8/V7/older save migration. `test_puzzles.c` checks 2457 legal three-rod Hanoi arrangements plus optimal solves for 3/4/5 rods × 3/5/7 disks, 31 games and dot pictures. `test_app.c` checks grouped 369/arithmetic menus, difficulty UI, five-rod Hanoi, notebook, shop and settings in Korean/English. It writes screenshots to `../qa_numberplay`. `test_save.c` checks old save files. SDL integration test mode does not alter the player's normal save.
