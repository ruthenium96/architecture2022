int main() {
	std::shared_prt<State> state_global = std::make_shared<StateGlobal>();           // vars, aliases, history, etc ...
	std::shared_prt<Streams> streams_global = std::make_shared<StreamsGlobal>();     // cin, cout, cerr
	
	ShellExecutor shell(state_global, streams_global);
	shell.execute();
	
	if (!state) {
		return state.get_errno();
	}
	
	return 0;
}

shell::execute() {
	while (this->running()) {    // until `exit` or ctrl+C
		std::string cmd_line = this->streams.read_line();
		CmdParser cmd_parser(cmd_line);
		StreamsLocal streams_local(streams);    // cin==streams.cin(), cout=std::ostringstream, cerr=streams.cerr()
		while (!cmd_parser.empty()) {
			std::shared_prt<Subcommand> current_subcommand = cmd_parser.get_next_subcommand(); 
			current_subcommand->preprocess();
			
			Program current_program = Program(current_subcommand);
		
			try {
				current_program.apply(this->state, streams_local);
			} catch (const CmdErrorException& e) {
				streams_local.cerr << e.what() << std::endl;
				this->stop();
			} catch (const ExitCmdException& e) {
				this->stop();
			}
			
			streams_local.cout_to_cin();
		}
		streams.write(streams_local);
		this->finish_cmd();
	}
}