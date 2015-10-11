{
	q->capacity = 5;
	q->input_index = 0;
	q->output_index = 0;

	q->packet_reordering[0] = 4;
	q->packet_reordering[1] = 3;
	q->packet_reordering[2] = 2;
	q->packet_reordering[3] = 1;
	q->packet_reordering[4] = 0;

	q->circular_buffer[0] = 0;
	q->circular_buffer[1] = 0;
	q->circular_buffer[2] = 0;
	q->circular_buffer[3] = 0;
	q->circular_buffer[4] = 0;
}

