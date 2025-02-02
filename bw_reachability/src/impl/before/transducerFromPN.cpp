Transducer::Transducer(pnTransition t) {
  int initial = 0;
  int counter = 0;
  for (int i = 0; i < t.first.size(); i++) {
    this->transitions.emplace(std::make_pair(counter, std::make_pair('1', '1')),
                              std::vector<int>{counter});
    int delta = t.second[i] - t.first[i];
    if (delta > 0) {
      for (int j = 0; j < t.first[i]; j++) {
        // TODO: ++counter
        this->transitions.emplace(
            std::make_pair(counter, std::make_pair('1', '1')),
            std::vector<int>{counter + 1});
        counter += 1;
      }
      for (int j = 0; j < delta; j++) {
        this->transitions.emplace(
            std::make_pair(counter, std::make_pair('X', '1')),
            std::vector<int>{counter + 1});
        counter += 1;
      }
      this->transitions.emplace(
          std::make_pair(counter, std::make_pair('T', 'T')),
          std::vector<int>{counter + 1});
      counter += 1;
    } else if (delta < 0) {
      for (int j = 0; j < t.second[i]; j++) {
        // TODO: ++counter
        this->transitions.emplace(
            std::make_pair(counter, std::make_pair('1', '1')),
            std::vector<int>{counter + 1});
        counter += 1;
      }
      for (int j = 0; j < (-delta); j++) {
        this->transitions.emplace(
            std::make_pair(counter, std::make_pair('1', 'X')),
            std::vector<int>{counter + 1});
        counter += 1;
      }
      this->transitions.emplace(
          std::make_pair(counter, std::make_pair('T', 'T')),
          std::vector<int>{counter + 1});
      counter += 1;
    } else {
    }
  }
}