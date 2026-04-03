(fn priority_parser [] (ox/log "priority_parser not implemented"))
(fn common_parser [] (ox/log "common_parser not implemented"))
(fn normal_parser [] (ox/log "normal_parser not implemented"))

(ox/register_fn "priority_parser" priority_parser)
(ox/register_fn "common_parser"   common_parser  )
(ox/register_fn "normal_parser"   normal_parser  )

(ox/log "Command Parsers module ready")
